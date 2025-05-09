/*
 *  Copyright (c) 2025,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

/*

Run-time dependencies
=====================

Help
----

.pkg-dep unisim/service/debug/nodejs/doc/repl
.pkg-dep unisim/service/debug/nodejs/doc/api

Third party Node.js/npm packages
--------------------------------

jschardet
iconv
emphasize
ansi-styles
markdown-it
markdown-it-terminal

*/

const Tabular = require(__dirname + '/tabular.js');

class Dbg
{
	static instance = new Dbg();
	
	// ANSI color codes
	static colors =
	{
		black :  0,
		red   :  1,
		green :  2,
		yellow:  3,
		blue  :  4,
		purple:  5,
		cyan  :  6,
		white :  7
	};
	
	// ANSI intensity codes
	static intensities =
	{
		normal: 0,
		bold  : 1,
		dim   : 2
	};
	
	/** Options
	 * 
	 * @typedef {Object} Options
	 * @property {*} [input] - input stream (default: process.stdin)
	 * @property {*} [output] - output stream (default: process.stdout)
	 */
	
	/** constructor
	 * 
	 * @param {Options} [options] - options
	 */
	constructor(options = {})
	{
		if(!inBuiltinREPL) this.input = options.input || process.stdin;
		this.output = options.output || process.stdout;
		this.current_processor = processors[0];
		this.inferior(0);
		this.breakpoints = [];
		this.watchpoints = [];
		this.search_paths = JSON.stringify(searchPaths);
		this.source_cache = new Map();
		this.x_fmt = 'x';
		this.x_size = 'w';
		this.x_repeat_count = 1;
		this.tab_width = 2;
		this.tabular_border = 2;
		
		// Settings
		this.settings =
		{
			language:
			{
				value: 'C',
				description: 'Current source language.'
			},
			endian:
			{
				enum: [ 'little', 'big' ],
				value: 'little',
				description: 'Endianness of target.'
			},
			pagination:
			{
				value: !inBuiltinREPL,
				description: 'Whether to paginate.'
			},
			'disassemble-next-line':
			{
				enum: [ 'on', '1', 'off', '0', 'auto' ],
				value: 'off',
				description: 'Whether to disassemble next source line or instruction when execution stops:\n\n' +
				'  - ON: display both source line and instruction;\n' +
				'  - AUTO: display source line, otherwise instruction when source line can\'t be displayed;\n' +
				'  - OFF: do not display neither source line nor instruction\n'
			},
			listsize:
			{
				value: 10,
				description: 'Number of source lines to list by default.'
			},
			print:
			{
				elements:
				{
					min: 1,
					value: 200,
					description: 'Limit on string chars or array elements to print.'
				},
				pretty:
				{
					value: false,
					description: 'Whether to print structures and arrays in an indented format with one member/element per line, or in a compact format.'
				},
				'raw-values':
				{
					value: false,
					description: 'Whether to print in raw form.'
				}
			},
			style:
			{
				enabled:
				{
					value: true,
					description: 'Whether styling is enabled.'
				},
				address:
				{
					foreground:
					{
						value: 'blue',
						description: 'Foreground color for an address.'
					},
					background:
					{
						value: 'none',
						description: 'Background color for an address.'
					},
					intensity:
					{
						value: 'normal',
						description: 'Intensity for an address.'
					}
				},
				'function':
				{
					foreground:
					{
						value: 'yellow',
						description: 'Foreground color for an function.'
					},
					background:
					{
						value: 'none',
						description: 'Background color for an function.'
					},
					intensity:
					{
						value: 'normal',
						description: 'Intensity for an function.'
					}
				},
				filename:
				{
					foreground:
					{
						value: 'green',
						description: 'Foreground color for an filename.'
					},
					background:
					{
						value: 'none',
						description: 'Background color for an filename.'
					},
					intensity:
					{
						value: 'normal',
						description: 'Intensity for an filename.'
					}
				}
			}
		};
		
		// Add simulator parameters and statistics as debugger settings in section "sim"
		this.add_simulator_variables_as_settings();
		
		// Check run-time dependencies with third party Node.js/npm packages
		try
		{
			require('jschardet'); 
		}
		catch(err)
		{
			console.warn('WARNING! No character set detection available: please run \'npm install jschardet\'');
		}
		try
		{
			require('iconv'); 
		}
		catch(err)
		{
			console.warn('WARNING! No character set conversion available: please run \'npm install iconv\'');
		}
		if(!inBuiltinREPL)
		{
			try
			{
				const { all, createEmphasize } = require('emphasize');
				const emphasize = createEmphasize(all);
				this.settings.language.enum = emphasize.listLanguages();
				if(this.output.isTTY) this.emphasize = emphasize;
			}
			catch(err)
			{
				console.warn('WARNING! No source code highlighting available: please run \'npm install emphasize\'');
			};
		}
		if(this.output.isTTY)
		{
			try
			{
				const ansi_styles = require('ansi-styles');
				const markdown_it = require('markdown-it');
				const markdown_it_terminal = require('markdown-it-terminal');

				const markdown_it_terminal_options =
				{
					styleOptions: {},
					unescape: true,
					indent: '  '
				};
				
				this.md = markdown_it();
				this.md.use(markdown_it_terminal, markdown_it_terminal_options);
			}
			catch(err)
			{
				console.warn('WARNING! No markdown rendering available for help: please run \'npm install ansi-styles markdown-it markdown-it-terminal\'');
			}
		}
		
		this.settings.style.enabled.value = !!this.output.isTTY;
		
		// Help
		this.help =
		{
			repl:
			{
				stepi: this.markdown_file('repl/stepi.md'),
				step: this.markdown_file('repl/step.md'),
				nexti: this.markdown_file('repl/nexti.md'),
				next: this.markdown_file('repl/next.md'),
				finish: this.markdown_file('repl/finish.md'),
				'return': this.markdown_file('repl/return.md'),
				'continue': this.markdown_file('repl/continue.md'),
				quit: this.markdown_file('repl/quit.md'),
				'break': this.markdown_file('repl/break.md'),
				watch: this.markdown_file('repl/watch.md'),
				info:
				{
					_: this.markdown_file('repl/info.md'),
					breakpoints: this.markdown_file('repl/info_breakpoints.md'),
					watchpoints: this.markdown_file('repl/info_watchpoints.md'),
					threads: this.markdown_file('repl/info_threads.md'),
					inferiors: this.markdown_file('repl/info_inferiors.md'),
					'all-registers': this.markdown_file('repl/info_all_registers.md'),
					frame: this.markdown_file('repl/info_frame.md')
				},
				'delete': this.markdown_file('repl/delete.md'),
				enable: this.markdown_file('repl/enable.md'),
				disable: this.markdown_file('repl/disable.md'),
				backtrace: this.markdown_file('repl/backtrace.md'),
				thread: this.markdown_file('repl/thread.md'),
				inferior: this.markdown_file('repl/inferior.md'),
				x: this.markdown_file('repl/x.md'),
				print: this.markdown_file('repl/print.md'),
				disassemble: this.markdown_file('repl/disassemble.md'),
				'symbol-file': this.markdown_file('repl/symbol_file.md'),
				directory: this.markdown_file('repl/directory.md'),
				list: this.markdown_file('repl/list.md'),
				frame: this.markdown_file('repl/frame.md'),
				set: this.markdown_file('repl/set.md'),
				show: this.markdown_file('repl/show.md'),
				whatis: this.markdown_file('repl/whatis.md')
			},
			api:
			{
				AddressHook         : this.markdown_file('api/address_hook.md'),
				API                 : this.markdown_file('api/api.md'),
				ArrayType           : this.markdown_file('api/array_type.md'),
				BaseType            : this.markdown_file('api/base_type.md'),
				BooleanType         : this.markdown_file('api/boolean_type.md'),
				Breakpoint          : this.markdown_file('api/breakpoint.md'),
				CharType            : this.markdown_file('api/char_type.md'),
				ClassType           : this.markdown_file('api/class_type.md'),
				CompositeType       : this.markdown_file('api/composite_type.md'),
				ConstType           : this.markdown_file('api/const_type.md'),
				DataObject          : this.markdown_file('api/data_object.md'),
				DebugEvent          : this.markdown_file('api/debug_event.md'),
				DebugSymbol         : this.markdown_file('api/debug_symbol.md'),
				Enumerator          : this.markdown_file('api/enumerator.md'),
				EnumType            : this.markdown_file('api/enum_type.md'),
				ExecutableBinaryFile: this.markdown_file('api/executable_binary_file.md'),
				FloatingPointType   : this.markdown_file('api/floating_point_type.md'),
				FormalParameter     : this.markdown_file('api/formal_parameter.md'),
				FunctionType        : this.markdown_file('api/function_type.md'),
				Globals             : this.markdown_file('api/global.md'),
				Hook                : this.markdown_file('api/hook.md'),
				IntegerType         : this.markdown_file('api/integer_type.md'),
				InterfaceType       : this.markdown_file('api/interface_type.md'),
				Member              : this.markdown_file('api/member.md'),
				NamedType           : this.markdown_file('api/named_type.md'),
				Pointer             : this.markdown_file('api/pointer.md'),
				PointerType         : this.markdown_file('api/pointer_type.md'),
				Processor           : this.markdown_file('api/processor.md'),
				Register            : this.markdown_file('api/register.md'),
				Field               : this.markdown_file('api/field.md'),
				SourceCodeBreakpoint: this.markdown_file('api/source_code_breakpoint.md'),
				SourceCodeHook      : this.markdown_file('api/source_code_hook.md'),
				SourceCodeLocation  : this.markdown_file('api/source_code_location.md'),
				StackFrameInfo      : this.markdown_file('api/stack_frame_info.md'),
				Statement           : this.markdown_file('api/statement.md'),
				StructureType       : this.markdown_file('api/structure_type.md'),
				Stub                : this.markdown_file('api/stub.md'),
				SubProgramBreakpoint: this.markdown_file('api/subprogram_breakpoint.md'),
				SubProgramHook      : this.markdown_file('api/subprogram_hook.md'),
				SubProgram          : this.markdown_file('api/subprogram.md'),
				Typedef             : this.markdown_file('api/typedef.md'),
				Type                : this.markdown_file('api/type.md'),
				UnionType           : this.markdown_file('api/union_type.md'),
				UnspecifiedType     : this.markdown_file('api/unspecified_type.md'),
				VolatileType        : this.markdown_file('api/volatile_type.md'),
				Watchpoint          : this.markdown_file('api/watchpoint.md'),
				UnisimObject        : this.markdown_file('api/unisim_object.md'),
				UnisimVariable      : this.markdown_file('api/unisim_variable.md')
			}
		};
		
		// completion rules
		this.completion_rules =
		[
			[ 'si' ],
			[ 'stepi' ],
			[ 'step' ],
			[ 'ni' ],
			[ 'nexti' ],
			[ 'next' ],
			[ 'finish' ],
			[ 'return' ],
			[ 'continue' ],
			[ 'quit' ],
			[ 'break', this.complete_functions.bind(this) ],
			[ 'watch' ],
			[ 'info' ],
			[ 'info', 'breakpoints' ],
			[ 'info', 'watchpoints' ],
			[ 'info', 'all-registers' ],
			[ 'info', 'threads' ],
			[ 'info', 'inferiors' ],
			[ 'info', 'frame' ],
			[ 'delete' ],
			[ 'enable' ],
			[ 'disable' ],
			[ 'thread' ],
			[ 'inferior' ],
			[ 'x' ],
			[ 'show' ],
			[ 'show', 'directories' ],
			[ 'set' ],
			[ 'set', this.complete_data_objects.bind(this), '=' ],
			[ 'set', this.complete_registers.bind(this), '=' ],
			[ 'set', 'variable', this.complete_data_objects.bind(this), '=' ],
			[ 'set', 'directories' ],
			[ 'print', this.complete_data_objects.bind(this) ],
			[ 'print', this.complete_registers.bind(this) ],
			[ 'whatis', this.complete_data_objects.bind(this) ],
			[ 'disassemble', this.complete_functions.bind(this) ],
			[ 'symbol-file' ],
			[ 'directory' ],
			[ 'list' ],
			[ 'frame' ],
			[ 'help' ],
			[ 'help', 'show', 'directories' ],
			[ 'help', 'set', 'directories' ]
		];
		
		// add completion rules for help
		this.add_help_completion_rules(this.help.repl);
		this.add_help_completion_rules(this.help.api);
		
		// add completion rules for set/show settings and corresponding help
		this.add_help_settings_completion_rules(true, [ 'set' ]);
		this.add_help_settings_completion_rules(true, [ 'show' ]);
		this.add_help_settings_completion_rules(false, [ 'help', 'set' ]);
		this.add_help_settings_completion_rules(false, [ 'help', 'show' ]);
	}
	
	/** Add simulator variables as debugger settings
	 *
	 * @param {Object} settings - debugger settings
	 * @param {UnisimObject|Array.<UnisimVariable>|UnisimVariable} sim_elem - simulator object, or variable array, or variable
	 * @param {string|number} key - key in debugger settings to assign
	 */
	add_simulator_variables_as_settings(settings, sim_elem, key)
	{
		if(settings === undefined) settings = this.settings;
		if(sim_elem === undefined)
		{
			if(global.simulator === undefined) return;
			sim_elem = simulator;
		}
		if(key === undefined) key = 'sim';
		if(sim_elem instanceof UnisimObject)
		{
			settings[key] = {}
			for(let prop_name of [ 'children', 'variables' ])
			{
				let sim_elem_prop = sim_elem[prop_name];
				for(let k in sim_elem_prop)
				{
					if(sim_elem_prop[k] !== undefined) this.add_simulator_variables_as_settings(settings[key], sim_elem_prop[k], k);
				}
			}
		}
		else if(sim_elem instanceof Array)
		{
			settings[key] = {}
			for(let k in sim_elem)
			{
				if(sim_elem[k] !== undefined) this.add_simulator_variables_as_settings(settings[key], sim_elem[k], k);
			}
		}
		else if(sim_elem instanceof UnisimVariable)
		{
			settings[key] = sim_elem;
		}
	}
	
	/** Generate settings as a markdown document
	 * 
	 * @param {Object} [settings] - a setting
	 * @param {string} [parent_setting_full_name] - full name of parent setting
	 * @param {number} [level] - level of setting in the hierarchy of settings
	 * @return {string} a markdown document
	 */
	settings_to_markdown(settings, parent_setting_full_name, level = 1)
	{
		let output = '';
		if(settings === undefined)
		{
			output += '#'.repeat(level) + ' Settings\n\n';
			output += 'Use the "set" and "show" commands to handle the settings.\n\n';
			output += this.settings_to_markdown(this.settings, '', level + 1);
		}
		else
		{
			const setting_names = Object.keys(settings);
			for(let i = 0; i < setting_names.length; ++i)
			{
				const setting_name = setting_names[i];
				let setting_full_name = parent_setting_full_name + (parent_setting_full_name ? ' ' : '') + setting_name;
				const setting = settings[setting_name];
				output += '#'.repeat(level) + ' ' + setting_full_name + '\n\n';
				if(setting.hasOwnProperty('value'))
				{
					output += setting.description + (setting.description.endsWith('.') ? '' : '.') + '\n\n';
					output += 'Default value: ' + ((typeof setting.value === 'boolean') ? (setting.value ? 'on' : 'off') : setting.value) + '\n\n';
					let enum_values;
					if(typeof setting.value === 'boolean')
					{
						enum_values = [ 'on', '1', 'off', '0' ];
					}
					else if((typeof setting.value === 'string') && setting.hasOwnProperty('enum'))
					{
						enum_values = setting.enum;
					}
					if(enum_values !== undefined)
					{
						output += 'Allowed values: ' + enum_values.slice(0, -1).map((e) => '"' + e + '"').join(', ') + ' or ' + enum_values.slice(-1).map((e) => '"' + e + '"') + '.\n\n';
					}
				}
				else
				{
					output += this.settings_to_markdown(setting, setting_full_name, level + 1);
				}
			}
		}
		return output;
	}
	
	/** Render a markdown document as a stylized text with ANSI escape codes
	 * 
	 * @param {string} text - a markdown document
	 * @return {string} a stylized text with ANSI escape codes
	 */
	markdown(text)
	{
		if(this.md !== undefined)
		{
			return this.md.render(text);
		}
		else
		{
			return text;
		}
	}
	
	/** Render a markdown document file as a stylized text with ANSI escape codes
	 * 
	 * @param {string} filename - a markdown document filename
	 * @return {string} a stylized text with ANSI escape codes
	 */
	markdown_file(filename)
	{
		const fs = require('fs');
		const text = fs.readFileSync(__dirname + '/doc/' + filename, { encoding: 'utf-8' });
		return this.markdown(text);
	}

	/** Style a text using a style
	 * 
	 * @param {string} text - text
	 * @param {string} style - style ('address', 'function', or 'filename')
	 * @return {string} a stylized text with ANSI escape codes
	 */
	styling(text, style)
	{
		if(this.settings.style.enabled.value)
		{
			const fg = this.settings.style[style].foreground.value;
			const bg = this.settings.style[style].background.value;
			const i = this.settings.style[style].intensity.value;
			if((fg !== 'none') || (bg !== 'none') || (i !== 'normal'))
			{
				return (((fg !== 'none') || (i !== 'normal')) ? '\x1b[' + ((i !== 'normal') ? (Dbg.intensities[i] + ';') : '') + (30 + Dbg.colors[fg]) + 'm' : '') +
				       ((bg !== 'none') ? '\x1b[' + (40 + Dbg.colors[bg]) + 'm' : '') +
				       text +
				       '\x1b[0m';
			}
		}
		return text;
	}
	
	/** Escape a string
	 * 
	 * @param {string} input - input string
	 * @param {number} direction - direction (>= 0: forward, < 0: backward)
	 * @param {boolean} abbreviated - whether input string is abbreviated (incomplete)
	 * @return {string} an escaped string
	 */
	escaped_string(input, direction = 0, abbreviated = false)
	{
		const start = (direction >= 0) ? 0 : ((input.length >= this.settings.print.elements.value) ? (input.length - this.settings.print.elements.value) : 0);
		const end = (direction >= 0) ? ((input.length >= this.settings.print.elements.value) ? this.settings.print.elements.value : input.length) : input.length;
		if(input.length > this.settings.print.elements.value) abbreviated = true;
		let str = (abbreviated && (direction < 0)) ? '..."' : '"';
		for(let i = start; i < end; ++i)
		{
			const ch = input.charCodeAt(i);
			str += Dbg.char_literal(ch);
		}
		str += (abbreviated && (direction >= 0)) ? '"...' : '"';
		return str;
	}
	
	/** Stringify an input
	 * 
	 * @param {*} input - an input
	 * @param {string} fmt - a format (one of 'xotdufacs' character set)
	 * @param {number} [indent_level] - level of indentation (default: 0)
	 * @return {string} a stylized string representation of input
	 */
	stringify(input, fmt, indent_level = 0)
	{
		if(input === undefined) return 'undefined';
		if(input === null) return 'null';
		if((typeof input === 'number') || (typeof input === 'bigint')) return input.toString();
		if(typeof input === 'string') return this.escaped_string(input);
		if(typeof input === 'boolean') return input.toString();
		if(input instanceof Pointer) return this.toHexAddress(input.get());
		if(Buffer.isBuffer(input))
		{
			let base = 10;
			let signed = true;
			let prefix = '';
			if(fmt !== undefined)
			{
				switch(fmt)
				{
					case 'x': base = 16; signed = false; prefix = '0x'; break;
					case 'o': base = 8; signed = false; break;
					case 't': base = 2; signed = false; break;
					case 'd': base = 10; break;
					case 'u': base = 10; signed = false; break;
					case 'f': break;
					case 'a': base = 16; signed = false; prefix = '0x'; break;
					case 'c': break;
					case 's': break;
					default:
						console.error('unsupported format ' + fmt);
						return;
				}
			}
			if(fmt === 'c')
			{
				const value = input[0];
				return (((value & 0x80) != 0) ? -(((value & 0x7f) ^ 0x7f) + 1) : value) + " '" + Dbg.char_literal(value) + "'";
			}
			else if(fmt === 'f')
			{
				return this.fromFloat(input).toString();
			}
			else
			{
				const little_endian = (this.settings.endian.value === 'little');
				let value = 0n;
				for(let i = 0; i < input.length; ++i)
				{
					const byte = little_endian ? input[input.length - 1 - i] : input[i];
					value = (value << 8n) | BigInt(byte);
				}
				if(base === 10)
				{
					const byte_size = input.length;
					const sign_mask = 1n << BigInt((8 * byte_size) - 1);
					const lsb_mask = sign_mask - 1n;
					if(signed && ((sign_mask & value) != 0)) value = -(((value & lsb_mask) ^ lsb_mask) + 1n);
					return value.toString();
				}
				else
				{
					const byte_size = input.length;
					const digits = ((fmt === 'd') || (fmt === 'u')) ? 0 : Math.ceil(((8 * byte_size) + (fmt === 'o' ? 3 : 0)) / Math.log2(base));
					return prefix + Dbg.toBase(value, base, digits);
				}
			}
		}
		const pretty = !!this.settings.print.pretty.value;
		let output = '';
		++indent_level;
		if(Array.isArray(input))
		{
			output += '[';
			if(input.length > 0)
			{
				output += pretty ? '\n' : ' ';
				const last = input.length - 1;
				for(let i = 0; i <= last; ++i)
				{
					if(pretty) output += ' '.repeat(indent_level * this.tab_width);
					output += this.stringify(input[i], fmt, indent_level);
					if(i !== last) output += ',';
					output += pretty ? '\n' : ' ';
				}
				--indent_level;
				if(pretty) output += ' '.repeat(indent_level * this.tab_width);
			}
			output += ']';
		}
		else
		{
			const keys = Object.keys(input);
			output += '{';
			if(keys.length > 0)
			{
				output += pretty ? '\n' : ' ';
				const last = keys.length - 1;
				for(let i = 0; i <= last; ++i)
				{
					const key = keys[i];
					if(pretty) output += ' '.repeat(indent_level * this.tab_width);
					output += key;
					output += ': ';
					output += this.stringify(input[key], fmt, indent_level);
					if(i !== last) output += ',';
					output += pretty ? '\n' : ' ';
				}
				--indent_level;
				if(pretty) output += ' '.repeat(indent_level * this.tab_width);
			}
			output += '}';
		}
		return output;
	}
	
	/** Paginate (asynchronously)
	 * 
	 * @param {Promise.<string>} promise - a promise which fulfillment value is the input string to paginate on screen
	 * @return {Promise} a promise
	 */
	paginate(promise)
	{
		return new Promise((resolve, reject) =>
		{
			promise.then(async (input) =>
			{
				if(input)
				{
					if(!inBuiltinREPL && this.settings.pagination.value && this.input.isTTY && this.output.isTTY)
					{
						// stdin is a TTY
						const space = ' ';
						const escape = '\u001b';
						const end = '\u001b\u005b\u0046';
						const lines = input.replace(/\n$/, '').split('\n');
						let line_idx = 0;
						while(line_idx < lines.length)
						{
							const rows = (this.output.rows || 25) - 1;
							for(let row = 0; (row < rows) && (line_idx < lines.length); ++row, ++line_idx)
							{
								this.output.write(lines[line_idx] + '\n');
							}
							
							if(line_idx < lines.length)
							{
								let read_ctrl_promise = new Promise((resolve, reject) =>
								{
									this.output.write('--Type <SPACE> for more, <ESCAPE> to quit, <END> to continue without paging--');
									const stdin_is_raw = process.stdin.isRaw;
									if(!stdin_is_raw) process.stdin.setRawMode(true);
									process.stdin.setEncoding('utf-8');
									process.stdin.resume();
									
									const process_data_fn = (c) =>
									{
										if((c === space)  ||
										   (c === escape) ||
										   (c === end))
										{
											process.stdin.removeListener('data', process_data_fn);
											process.stdin.removeListener('error', process_error_fn);
											if(!stdin_is_raw) process.stdin.setRawMode(false);
											resolve(c);
										}
									};
									
									const process_error_fn = (error) =>
									{
										process.stdin.removeListener('data', process_data_fn);
										process.stdin.removeListener('error', process_error_fn);
										if(!stdin_is_raw) process.stdin.setRawMode(false);
										reject(error); // call reject callback
									};
									
									process.stdin.on(
										'data',
										process_data_fn
									).on(
										'error',
										process_error_fn
									);
								});
								
								try
								{
									const c = await read_ctrl_promise;
									this.output.cursorTo(0); // go to the beginning of line
									this.output.clearLine(); // clear the entire line
									if(c === escape) // escape
									{
										// stop
										break;
									}
									else if(c === end)
									{
										// continue without paging
										while(line_idx < lines.length)
										{
											this.output.write(lines[line_idx] + '\n');
											++line_idx;
										}
									}
								}
								catch(err)
								{
									reject(err);
									return;
								}
							}
						}
					}
					else
					{
						this.output.write(input);
					}
				}
				
				resolve();
			}).catch((err) =>
			{
				this.output.write(err.message + '\n');
				resolve();
			});
		});
	}
	
	/** Show help
	 * 
	 * @param {string} path - a list of keys separated with space
	 */
	show_help(path)
	{
		if(path)
		{
			const keys = path.trim().split(/\s+/);
			if(keys.length > 0)
			{
				const key = keys[0];
				if((key === 'set') || (key === 'show'))
				{
					if(keys.length > 1)
					{
						let output = '';
						let obj = this.settings;
						for(let i = 1; (i < keys.length) && (obj !== undefined); obj = obj[keys[i]], ++i);
						if((typeof obj === 'object') && obj.hasOwnProperty('description'))
						{
							const setting_name = keys.slice(1).join(' ');
							output += '# "' + keys.join(' ') + '" command\n\n';
							output += key.charAt(0).toUpperCase() + key.slice(1) + ' setting "' + setting_name + '".\n\n';
							output += 'Description: ' + obj.description.charAt(0).toLowerCase() + obj.description.slice(1) + (obj.description.endsWith('.') ? '' : '.') +'\n';
							let enum_values;
							if(typeof obj.value === 'boolean')
							{
								enum_values = [ 'on', '1', 'off', '0' ];
							}
							else if((typeof obj.value === 'string') && obj.hasOwnProperty('enum'))
							{
								enum_values = obj.enum;
							}
							if(enum_values !== undefined)
							{
								output += '\nAllowed values: ' + enum_values.slice(0, -1).map((e) => '"' + e + '"').join(', ') + ' or ' + enum_values.slice(-1).map((e) => '"' + e + '"') + '.\n';
							}
							this.log(this.markdown(output));
							return;
						}
						if((keys[1] === 'directories') || (keys[1] === 'dir'))
						{
							const setting_name = keys.slice(1).join(' ');
							output += '# "' + keys.join(' ') + '" command\n';
							output += key.charAt(0).toUpperCase() + key.slice(1) + ' setting "' + setting_name + '".\n\n';
							output += 'Description: ' + key.charAt(0).toUpperCase() + key.slice(1) + ' source directories searched.\n';
							this.log(this.markdown(output));
							return;
						}
					}
				}
				else if(key !== 'help')
				{
					for(let help of [ this.help.repl, this.help.api ])
					{
						for(let i = 0; (i < keys.length) && (keys[i] !== '_') && (typeof help === 'object'); help = help[keys[i]], ++i);
						if(typeof help === 'string')
						{
							this.log(help);
							return;
						}
						if((typeof help === 'object') && (typeof help._ === 'string'))
						{
							this.log(help._);
							return;
						}
					}
					this.log('No help for ' + keys.join(' ') + '.');
				}
			}
		}
		this.log('Type help <item> for help, where item is either a command or a Javascript API item.\n\nAvailable commands are:');
		let tab = new Tabular({ columns: 6, border: this.tabular_border });
		let commands = Object.keys(this.help.repl);
		commands.push('set', 'show');
		commands.sort();
		for(let command of commands) tab.write(command);
		this.log(tab.flush());
		this.log('\nAvailable items for Javascript API:');
		let api_items = Object.keys(this.help.api);
		api_items.sort();
		for(let api_item of api_items) tab.write(api_item);
		this.log(tab.flush());
	}
	
	/** Load debug info from a file
	 * 
	 * @param {string} file - a filename
	 */
	symbol_file(file)
	{
		loadDebugInfo(file);
		
		if(this.current_exec_bin_file === undefined)
		{
			const exec_bin_files = getExecutableBinaryFiles();
			if(exec_bin_files.length > 0) this.current_exec_bin_file = exec_bin_files[0];
		}
	}
	
	/** Add directory to source search path
	 * 
	 * @param {string} dirname - a directory name
	 */
	directory(dirname)
	{
		if(dirname)
		{
			searchPaths.push(dirname);
		}
		else
		{
			while(searchPaths.length > 0) searchPaths.pop();
		}
	}
	
	/** Set current thread
	 * 
	 * @param {number} id - thread id
	 */
	thread(id)
	{
		if(id < processors.length)
			this.current_processor = processors[id];
		else
			console.error('Invalid thread id ' + id);
	}
	
	inferior(id)
	{
		const exec_bin_files = getExecutableBinaryFiles();
		let found = false;
		for(let exec_bin_file of exec_bin_files)
		{
			if(exec_bin_file.id === id)
			{
				exec_bin_file.enable = true;
				this.current_exec_bin_file = exec_bin_file;
				found = true;
			}
			else
			{
				exec_bin_file.enable = false;
			}
		}
		if(!found)
		{
			console.error('Invalid inferior id ' + id);
		}
	}
	
	/** Set setting
	 * 
	 * @param {string} path - a list of keys separated with space
	 * @param {integer|string|boolean} value - a value
	 */
	set_setting(path, value)
	{
		let obj = this.settings;
		const keys = path.trim().split(/\s+/);
		for(let i = 0; (i < keys.length) && (obj !== undefined); obj = obj[keys[i]], ++i);
		if((keys.length > 0) && (typeof obj === 'object') && obj.hasOwnProperty('description') && obj.hasOwnProperty('value'))
		{
			switch(typeof obj.value)
			{
				case 'number':
					const number_value = Number(value.toString());
					if(Number.isInteger(number_value))
					{
						if(obj.hasOwnProperty('min') && (number_value < obj.min))
						{
							console.error('>= ' + obj.min + ' expected.');
							return;
						}
						if(obj.hasOwnProperty('max') && (number_value > obj.max))
						{
							console.error('<= ' + obj.max + ' expected.');
							return;
						}
						obj.value = number_value;
						return;
					}
					console.error('integer expected.');
					break;
				case 'boolean': 
					if((Number(value) === 1) || (value.toString().toLowerCase() === 'on'))
					{
						obj.value = true;
						return;
					}
					if((Number(value) === 0) || (value.toString().toLowerCase() === 'off'))
					{
						obj.value = false;
						return;
					}
					console.error('"on" or "off" expected.');
					break;
				case 'string':
					if(obj.hasOwnProperty('enum') && (obj.enum instanceof Array) && (obj.enum.length > 0))
					{
						for(let e of obj.enum)
						{
							if(value.toString().toLowerCase() === e.toLowerCase())
							{
								obj.value = e.toLowerCase();
								return;
							}
						}
						console.error(obj.enum.slice(0, -1).map((e) => '"' + e + '"').join(', ') + ' or ' + obj.enum.slice(-1).map((e) => '"' + e + '"') + ' expected.');
						return;
					}
					obj.value = value.toString();
					return;
				default:
					console.error('Internal error!');
					return;
			}
		}
		else
		{
			console.error('No setting "' + path + '"');
		}
	}
	
	/** Display a setting
	 * 
	 * @param {string} path - a list of keys separated with space
	 */
	show_setting(path)
	{
		let obj = this.settings;
		const keys = path.trim().split(/\s+/);
		for(let i = 0; (i < keys.length) && (obj !== undefined); obj = obj[keys[i]], ++i);
		if((keys.length > 0) && (typeof obj === 'object') && obj.hasOwnProperty('description') && obj.hasOwnProperty('value'))
		{
			this.log(keys.join(' ') + ': ' + ((typeof obj.value === 'boolean') ? (obj.value ? 'on' : 'off') : obj.value));
			this.log(obj.description);
		}
		else if((path === 'directories') || (path === 'dir'))
		{
			this.log('Source directories searched: \n' + searchPaths.join('\n'));
		}
		else
		{
			console.error('Undefined show command: "' + path + '".');
		}
	}
	
	/** Get program counter value
	 * 
	 * @param {Processor} processor - the processor
	 * @return {bigint} the value of program counter
	 */
	static get_pc(processor)
	{
		return BigInt(processor.registers[processor.programCounterName].get()) * BigInt(processor.memoryAtomSize);
	}
	
	/** Convert byte address to a display address (for user)
	 * 
	 * @param {bigint} addr - byte address
	 * @return {bigint} display address
	 */
	static display_addr(addr)
	{
		return addr / BigInt(Dbg.instance.current_processor.memoryAtomSize);
	}
	
	/** Convert a user address to a byte address
	 * 
	 * @param {bigint} addr_text - display address
	 * @return {bigint} a byte address
	 */
	static user_addr(addr_text)
	{
		return BigInt(addr_text) * BigInt(Dbg.instance.current_processor.memoryAtomSize);
	}
	
	/** Convert address to a stylized function name
	 * 
	 * @param {bigint} addr - an address
	 * @return {string} a stylized string with function name
	 */
	toFunctionName(addr)
	{
		const subprogram = findSubProgram(addr);
		if(subprogram !== undefined)
		{
			return this.styling(subprogram.cdecl, 'function');
		}
		else
		{
			const sym = findSymbol(addr, 'function');
			if(sym !== undefined)
				return this.styling(sym.name, 'function');
			else
				return '?';
		}
	}
	
	/** Convert address to a stylized function name + offset
	 * 
	 * @param {bigint} addr - an address
	 * @return {string} a stylized string with function name + offset
	 */
	toFunctionNamePlusOffset(addr)
	{
		const subprogram = findSubProgram(addr);
		if(subprogram !== undefined)
		{
			return this.styling(subprogram.cdecl, 'function') + '+' + Dbg.display_addr(addr - BigInt(subprogram.address));
		}
		else
		{
			const sym = findSymbol(addr, 'function');
			if(sym !== undefined)
				return this.styling(sym.name, 'function') + '+' + Dbg.display_addr(addr - BigInt(sym.address));
			else
				return '?';
		}
	}
	
	/** Stylized a filename
	 * 
	 * @param {string} filename - a filename
	 * @return {string} a stylized filename string
	 */
	toFilename(filename)
	{
		return this.styling(filename, 'filename');
	}
	
	/** Convert an address to a stylized hexadecimal string
	 * 
	 * @param {bigint} value - an address
	 * @return {string} a stylized hexadecimal string
	 */
	toHexAddress(value)
	{
		return this.styling('0x' + Dbg.toHex(Dbg.display_addr(value), 16), 'address');
	}
	
	/** Convert a value to an hexadecimal string
	 * 
	 * @param {bigint} value - an integer value
	 * @param {number} digits - minimum of digits (zero means as strictly needed)
	 * @return {string} an hexadecimal string
	 */
	static toHex(value, digits = 0)
	{
		return Dbg.toBase(value, 16, digits);
	}
	
	/** Convert a value to a base
	 * 
	 * @param {bigint} value - an integer value
	 * @param {number} [base] - base (e.g. 2, 10, or 16)
	 * @param {number} [digits] - minimum of digits (zero means as strictly needed)
	 * @return {string} an hexadecimal string
	 */
	static toBase(value, base = 10, digits = 0)
	{
		return digits ? (('0'.repeat(digits) + value.toString(base)).slice(-digits)) : value.toString(base);
	}
	
	/** Convert a unicode codepoint to a C character literal
	 * 
	 * @param {number} value - a unicode codepoint
	 * @return {string} a C character literal
	 */
	static char_literal(value)
	{
		switch(value)
		{
			case 0x27: return "\\'";
			case 0x22: return '\\"';
			case 0x3f: return "\\?";
			case 0x5c: return "\\\\";
			case 0x07: return "\\a";
			case 0x08: return "\\b";
			case 0x0c: return "\\f";
			case 0x0a: return "\\n";
			case 0x0d: return "\\r";
			case 0x09: return "\\t";
			case 0x0b: return "\\v";
		}
		return ((value < 32) || ((value >= 127) && (value <= 255))) ? ('\\' + Dbg.toBase(value, 8, 3)) : String.fromCharCode(value);
	}
	
	/** Convert a string to a Javascript unicode literal string
	 * 
	 * @param {string} text - a text
	 * @return {string} a literal string
	 */
	static to_unicode_literal(text)
	{
		let res = '';
		for(let i = 0; i < text.length; ++i)
		{
			res += '\\u' + Dbg.toHex(text.charCodeAt(i), 4);
		}
		return res;
	}
	
	/** Convert an IEEE 754 floating-point to a number
	 * 
	 * @param {Buffer} buf - a raw buffer
	 * @return {number} a number
	 */
	fromFloat(buf)
	{
		if(!Buffer.isBuffer(buf) || ((buf.length != 4) && (buf.length != 8))) throw new TypeError('Expecting a 4-byte or 8-byte Buffer');
		const array_buf = new ArrayBuffer(buf.length);
		const view = new Uint8Array(array_buf);
		for(let i = 0; i < buf.length; ++i) view[i] = buf[i];
		const data_view = new DataView(array_buf);
		const little_endian = this.settings.endian.value === 'little';
		return (buf.length === 8) ? data_view.getFloat64(0, little_endian)
		                          : data_view.getFloat32(0, little_endian);
	}
	
	/** Read (asynchronously) some lines of source code
	 * 
	 * @param {string] source_path - filename to read
	 * @param {number} lineno - starting line number
	 * @param {number} count - number of lines to read
	 * @return {Promise.<string>} a promise
	 */
	source(source_path, lineno, count)
	{
		return new Promise((resolve, reject) =>
		{
			const search_paths = JSON.stringify(searchPaths);
			if(this.search_paths != search_paths)
			{
				this.source_cache.clear();
				this.search_paths = search_paths;
			}
			
			let src = this.source_cache.get(source_path);
			if(src === undefined)
			{
				const resolved_source_path = locateFile(source_path, { lazyMatch: true });
				
				if(resolved_source_path !== undefined)
				{
					const fs = require('node:fs');
					try
					{
						const raw_source_code = fs.readFileSync(resolved_source_path);
			
						let source_code;
						try
						{
							const jschardet = require('jschardet');
							const { Iconv } = require('iconv');
							const detected = jschardet.detect(raw_source_code);
							const encoding = detected.encoding || detected[0].encoding;
							const iconv = new Iconv(encoding, 'UTF-8');
							source_code = iconv.convert(raw_source_code).toString();
						}
						catch(err)
						{
							source_code = raw_source_code.toString();
						}
						let highlighted_source_code = (this.emphasize !== undefined) ? this.emphasize.highlight(this.settings.language.value, source_code).value : source_code;
						let lines = highlighted_source_code.split(/\r?\n|\r|\n/g);
						src = { lines, resolved_source_path };
					}
					catch(err)
					{
						src = {};
					}
				}
				else
				{
					src = {};
				}
				
				this.source_cache.set(source_path, src);
			}
			
			if(src.resolved_source_path === undefined)
			{
				reject(new Error('source file "' + source_path + '" not found'));
				return;
			}
			
			if((lineno < 1) || ((lineno - 1) >= src.lines.length))
			{
				reject(new Error('Line number ' + lineno + ' out of range; ' + source_path + ' has ' + src.lines.length + ' lines.'));
				return;
			}
			
			let output = '';
			let max_count = src.lines.length - lineno + 1;
			if((count == 0) || (count >= max_count)) count = max_count;
			for(let i = 0; i < count; ++i, ++lineno)
			{
				output += lineno + ' ' + src.lines[lineno - 1] + '\n';
			}
			
			resolve(output);
		});
	}
	
	static count_linefeeds(text)
	{
		return text.length - text.replace(/\n/g, '').length;
	}
	
	/** List specified function or line
	 * 
	 * @param {bigint|string} loc - location (string format: '-', '+', '.', LINENUM, FILE:LINENUM, FUNCTION, FILE:FUNCTION)
	 */
	list(loc)
	{
		return new Promise(async (resolve, reject) =>
		{
			let file;
			let lineno;
			if(loc === undefined)
			{
				file = this.list_file;
				lineno = this.list_next_lineno;
			}
			else if(typeof loc === 'string')
			{
				loc = loc.trim();
				if(loc === '-')
				{
					if(this.list_file !== undefined) file = this.list_file;
					lineno = ((this.settings.listsize.value !== 0) && (this.list_lineno !== undefined) && (this.list_lineno > this.settings.listsize.value)) ? (this.list_lineno - this.settings.listsize.value) : 1;
				}
				else if((loc.length === 0) || (loc === '+'))
				{
					if(this.list_file !== undefined) file = this.list_file;
					if(this.list_next_lineno !== undefined) lineno = this.list_next_lineno;
				}
				else if(loc === '.')
				{
					const pc = Dbg.get_pc(this.current_processor);
					const stmt = findStatement(pc, { scope: 'nearest-lower-or-equal-stmt-within-function' });
					if(stmt !== undefined)
					{
						file = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
						const middle = (this.settings.listsize.value / 2) || 5;
						lineno = (stmt.lineno > middle) ? (stmt.lineno - middle) : 1;
					}
					else
					{
						reject(new Error('No current source file'));
						return;
					}
				}
				else
				{
					const pos = loc.indexOf(':');
					if(pos >= 0)
					{
						file = loc.slice(0, pos);
						loc = loc.slice(pos + 1);
					}
					else
					{
						const pc = Dbg.get_pc(this.current_processor);
						const stmt = findStatement(pc, { scope: 'nearest-lower-or-equal-stmt-within-function' });
						if(stmt !== undefined)
						{
							file = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
						}
						else
						{
							reject(new Error('No current source file'));
							return;
						}
					}
					
					if(loc.match(/^\d+$/) !== null)
					{
						lineno = Number(loc);
						if(!Number.isInteger(lineno) || (lineno < 1))
						{
							reject(new Error('Expected a line number >= 1'));
							return;
						}
					}
					else
					{
						let addr;
						const subprogram = findSubProgram(loc, { compilationUnit: file });
						if(subprogram !== undefined)
						{
							addr = BigInt(subprogram.address);
						}
						else
						{
							const sym = findSymbol(loc, 'function');
							if(sym !== undefined)
							{
								addr = BigInt(sym.address);
							}
						}
						
						const stmt = findStatement(addr, { scope: 'nearest-lower-or-equal-stmt-within-function' });
						if(stmt !== undefined)
						{
							file = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
							lineno = stmt.lineno;
						}
					}
				}
			}
			else
			{
				let addr = BigInt(loc);
				const stmt = findStatement(addr, { scope: 'nearest-lower-or-equal-stmt-within-function' });
				if(stmt !== undefined)
				{
					file = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
					lineno = stmt.lineno;
				}
			}
			
			let output=''
			
			if((file === undefined) || (lineno === undefined))
			{
				const pc = Dbg.get_pc(this.current_processor);
				const stmt = findStatement(pc, { scope: 'nearest-lower-or-equal-stmt-within-function' });
				if(stmt !== undefined)
				{
					file = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
					lineno = stmt.lineno;
				console.warn(file);
				console.warn(lineno);
				}
				else
				{
					reject(new Error('No current source file'));
					return;
				}
			}
			
			try
			{
				output = await this.source(file, lineno, this.settings.listsize.value);
				this.list_file = file;
				this.list_lineno = lineno;
				this.list_next_lineno = lineno + Dbg.count_linefeeds(output);
			}
			catch(err)
			{
				console.error(err.message);
			}
			resolve(output);
		});
	}

	/** Examine memory (asynchronously)
	 * 
	 * @param {string|bigint} loc - a location
	 * @param {number} repeat_count - repeat count
	 * @param {string} fmt - format (one of characters 'xotdufacsi')
	 * @param {string} size - a size (one of characters 'bhwg')
	 * @return {Promise.<string>} a promise
	 */
	examine_memory(loc, repeat_count, fmt, size)
	{
		return new Promise(async (resolve, reject) =>
		{
			let addr;
		
			if(loc === undefined)
			{
				if(this.x_addr === undefined)
				{
					reject(new Error("Argument required (starting display address)."));
					return;
				}
				addr = this.x_addr;
			}
			else if(typeof loc === 'string')
			{
				const sym = findSymbol(loc, 'function');
				if(sym !== undefined)
				{
					addr = BigInt(sym.address);
				}
				else if(loc.match(/^\s*\$/) !== null)
				{
					const m = loc.match(/^\s*\$([^\s]+)\s*$/);
					if((m !== null) && (m.length >= 2))
					{
						const reg_name = m[1];
						if(this.current_processor.registers.hasOwnProperty(reg_name))
						{
							const reg = this.current_processor.registers[reg_name];
							addr = reg.get();
						}
						else
						{
							reject(new Error('Register ' + reg_name + ' does not exist'));
							return;
						}
					}
				}
				else
				{
					reject(new Error('No symbol loaded for "' + loc + '".'));
					return;
				}
			}
			else if(typeof loc !== 'bigint')
			{
				addr = BigInt(loc);
			}
			else
			{
				addr = loc;
			}
			
			if(repeat_count === undefined) repeat_count = this.x_repeat_count;
			const count = (repeat_count >= 0) ? repeat_count: -repeat_count;
			let base;
			let signed = true;
			let prefix = '';
			let byte_size;
			if(size === undefined) size = this.x_size;
			switch(size)
			{
				case 'b': byte_size = 1; break;
				case 'h': byte_size = 2; break;
				case 'w': byte_size = 4; break;
				case 'g': byte_size = 8; break;
				default:
					console.error('unsupported size ' + size);
					return;
			}
			if(byte_size < this.current_processor.memoryAtomSize) byte_size = this.current_processor.memoryAtomSize;
			if(fmt === undefined) fmt = this.x_fmt;
			switch(fmt)
			{
				case 'x': case 'z': base = 16; signed = false; prefix = '0x'; break;
				case 'o': base = 8; signed = false; break;
				case 't': base = 2; signed = false; break;
				case 'd': base = 10; break;
				case 'u': base = 10; signed = false; break;
				case 'f':
					if((byte_size === 4) || (byte_size === 8)) break;
					base = 10; signed = true; fmt = 'd'; break;
				case 'a': base = 16; signed = false; prefix = '0x'; break;
				case 'c': break;
				case 's': break;
				case 'i': break;
				default:
					console.error('unsupported format ' + fmt);
					return;
			}
			const columns = ((fmt === 's') || (fmt === 'i')) ? 1 : (16 / byte_size);
			let tab = new Tabular({ columns: (columns + 2), border: this.tabular_border });
			if(fmt === 's')
			{
				const { StringDecoder } = require('node:string_decoder');
				const decoder = new StringDecoder('utf8');
				const step = BigInt((repeat_count >= 0) ? 1 : -1) * BigInt(this.current_processor.memoryAtomSize);
				const max_byte_length = 4 * (this.settings.print.elements.value || 65536);
				for(let idx = 0; idx < count; ++idx)
				{
					let string_addr;
					if(repeat_count >= 0) string_addr = addr;
					let array_buf = new ArrayBuffer(0, { maxByteLength: max_byte_length });
					let view = new Uint8Array(array_buf);
					
					let end_of_string = false;
					let unterminated_string = false;
					do
					{
						let buf;
						try
						{
							buf = this.current_processor.readMemory(addr, 1);
						}
						catch(err)
						{
						}
						if(buf !== undefined)
						{
							for(let byte of buf)
							{
								if(byte === 0)
								{
									end_of_string = true;
									break;
								}
								array_buf.resize(array_buf.byteLength + 1);
								view[view.byteLength - 1] = byte;
								if(array_buf.byteLength === array_buf.maxByteLength)
								{
									unterminated_string = true;
									end_of_string = true;
									break;
								}
							}
						}
						else
						{
							unterminated_string = true;
							end_of_string = true;
						}
						if(!end_of_string && (repeat_count < 0)) string_addr = addr;
						addr += step;
					}
					while(!end_of_string);
					
					if(repeat_count < 0) view.reverse();
					let string = this.escaped_string(decoder.end(view), repeat_count, unterminated_string);
					tab.write(this.toHexAddress(string_addr));
					tab.write('<' + this.toFunctionNamePlusOffset(string_addr) + '>:');
					tab.write(string);
				}
			}
			else if(fmt === 'i')
			{
				let info = {};
				for(let idx = 0; idx < count; ++idx)
				{
					tab.write(this.toHexAddress(addr));
					tab.write('<' + this.toFunctionNamePlusOffset(addr) + '>:');
					tab.write(this.current_processor.disasm(addr, info));
					
					addr = BigInt(info.nextAddr);
				}
			}
			else
			{
				const step = BigInt((repeat_count >= 0) ? byte_size : -byte_size);
				const digits = ((fmt === 'd') || (fmt === 'u')) ? 0 : Math.ceil(((8 * byte_size) + (fmt === 'o' ? 3 : 0)) / Math.log2(base));
				let col = 0;
				for(let idx = 0; idx < count; ++idx, addr += step)
				{
					if(col === 0)
					{
						tab.write(this.toHexAddress(addr));
						tab.write('<' + this.toFunctionNamePlusOffset(addr) + '>:');
					}
					let buf;
					try
					{
						buf = this.current_processor.readMemory(addr, byte_size);
					}
					catch(err)
					{
					}
					if(buf !== undefined)
					{
						if(fmt === 'c')
						{
							const value = buf[0];
							tab.write((((value & 0x80) != 0) ? -(((value & 0x7f) ^ 0x7f) + 1) : value) + " '" + Dbg.char_literal(value) + "'");
						}
						else if(fmt === 'f')
						{
							tab.write(this.fromFloat(buf));
						}
						else
						{
							const little_endian = (this.settings.endian.value === 'little');
							let value = 0n;
							for(let i = 0; i < buf.length; ++i)
							{
								const byte = little_endian ? buf[buf.length - 1 - i] : buf[i];
								value = (value << 8n) | BigInt(byte);
							}
							if(base === 10)
							{
								const sign_mask = 1n << BigInt((8 * byte_size) - 1);
								const lsb_mask = sign_mask - 1n;
								if(signed && ((sign_mask & value) != 0)) value = -(((value & lsb_mask) ^ lsb_mask) + 1n);
								tab.write(value);
							}
							else
							{
								tab.write(prefix + Dbg.toBase(value, base, digits));
							}
						}
					}
					else
					{
						tab.write('?'.repeat(digits || 1));
					}
					if(++col === columns) col = 0;
				}
			}
			this.x_addr = addr;
			if(repeat_count !== undefined) this.x_repeat_count = repeat_count;
			if(fmt !== undefined) this.x_fmt = fmt;
			if(size !== undefined) this.x_size = size;
			resolve(tab.flush());
		});
	}
	
	/** Set value of a program variable
	 * 
	 * @param {string} expr - an expression (e.g. '*x->t[2].z')
	 * @param {*} value - a value
	 */
	set_variable(expr, value)
	{
		let data_object = new DataObject(this.current_processor, expr);
		try
		{
			data_object.set(value);
		}
		catch(err)
		{
			console.error(err.message);
		}
	}
	
	set_register(reg_name, value)
	{
		if(this.current_processor.registers.hasOwnProperty(reg_name))
		{
			this.current_processor.registers[reg_name].set(value);
		}
		else
		{
			console.error('No register ' + reg_name);
		}
	}
	
	/** Print (asynchronously) a variable
	 * 
	 * @param {string} expr - an expression (e.g. '*x->t[2].z' or '&*x->t[2].z')
	 * @param {string} [fmt] - a format (one of 'xotdufacs' character set)
	 * @return {Promise.<string>} a promise
	 */
	print(expr, fmt)
	{
		return new Promise((resolve, reject) =>
		{
			const printing_register = (expr.match(/^\s*\$/) !== null);
			if(printing_register)
			{
				const m = expr.match(/^\s*\$([^\s]+)\s*$/);
				if((m !== null) && (m.length >= 2))
				{
					const reg_name = m[1];
					if(this.current_processor.registers.hasOwnProperty(reg_name))
					{
						const reg = this.current_processor.registers[reg_name];
						
						let base = 10;
						let signed = true;
						let prefix = '';
						let digits = 0;
						if(fmt === undefined) fmt = 'd';
						switch(fmt)
						{
							case 'x': base = 16; signed = false; prefix = '0x'; break;
							case 'z': base = 16; signed = false; prefix = '0x'; digits = 2 * reg.size; break;
							case 'o': base = 8; signed = false; break;
							case 't': base = 2; signed = false; break;
							case 'd': base = 10; break;
							case 'u': base = 10; signed = false; break;
							case 'f': break;
							case 'a': base = 16; signed = false; prefix = '0x'; break;
							case 'c': break;
							case 's': break;
							default:
								reject('unsupported format ' + fmt);
								return;
						}
						const value = reg.get({ signed });
						resolve(prefix + Dbg.toBase(value, base, digits) + '\n');
					}
					else
					{
						reject(new Error('Register ' + reg_name + ' does not exist'));
					}
				}
				else
				{
					reject(new Error('Got an empty register name'));
				}
			}
			else
			{
				const printing_address = (expr.match(/^\s*\&/) !== null);
				let data_object = new DataObject(this.current_processor, printing_address ? expr.replace(/^\s*\&/, '') : expr);
				if(printing_address)
				{
					if(data_object.address !== undefined)
					{
						resolve(this.toHexAddress(data_object.address) + '\n');
					}
					else
					{
						reject(new Error(data_object.name + ' has no address'));
					}
				}
				else
				{
					try
					{
						if(this.settings.print['raw-values'].value) fmt = (fmt || '') + 'x';
						const get_options =
						{
							raw: (fmt !== undefined)
						};
						const value = data_object.get(get_options);
						resolve(this.stringify(value, fmt) + '\n');
					}
					catch(err)
					{
						reject(new Error(err));
					}
				}
			}
		});
	}
	
	whatis(expr)
	{
		return new Promise((resolve, reject) =>
		{
			let data_object = new DataObject(this.current_processor, expr);
			try
			{
				const type = data_object.type;
				resolve(type.cdecl + '\n');
			}
			catch(err)
			{
				reject(new Error(err));
			}
		});
	}
	
	/** Disassemble options
	 * 
	 * @typedef {Object} DisassembleOptions
	 * @property {boolean} [enable_cursor] - whether to enable cursor at program counter
	 * @property {string} [message] - a contextual message to print before disassembly
	 */
	
	/** Disassemble (asynchronously) between an address range
	 * 
	 * @param {bigint} start - a starting address
	 * @param {bigint} end - a end address (excluded)
	 * @param {string } [flags] - flags (combination of character set 'srbn')
	 * @param {DisassembleOptions} [options] - disassemble options
	 * @return {Promise.<string>} a promise
	 */
	disassemble_address_range(start, end, flags, options = {})
	{
		return new Promise(async (resolve, reject) =>
		{
			if(start >= end)
			{
				reject(new Error('invalid address range'));
				return;
			}
			const flag_s = (flags !== undefined) && (flags.indexOf('s') >= 0);
			const flag_r = (flags !== undefined) && (flags.indexOf('r') >= 0);
			const flag_b = (flags !== undefined) && (flags.indexOf('b') >= 0);
			const flag_n = (flags !== undefined) && (flags.indexOf('n') >= 0);
			if(flag_r && flag_b)
			{
				reject(new Error('/r and /b can\'t be mixed'));
				return;
			}
			let output = '';
			const pc = Dbg.get_pc(this.current_processor);
			const print_cursor = (!!options.enable_cursor && (pc >= start) && (pc < end));
			let addr = start;
			let info = {};
			let last_source_path;
			let last_source_output = '';
			const columns = 3 + print_cursor + (flag_r || flag_b);
			let tab = new Tabular({ columns, border: this.tabular_border });
			if(options.hasOwnProperty('message'))
			{
				if(options.message) output += options.message + '\n';
			}
			else
				output += 'Dump of assembler code from ' + this.toHexAddress(start) + ' to ' + this.toHexAddress(end) + ':\n';
			
			while(addr < end)
			{
				let has_stmt = false;
				let next_stmt_addr;
				
				if(flag_s || flag_n)
				{
					const stmt = findStatement(addr, { scope: (addr === start) ? 'nearest-lower-or-equal-stmt-within-function' : 'exact-stmt' });
					if(stmt !== undefined)
					{
						let source_output = '';
						const lineno = stmt.lineno;
						const source_path = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
						if(last_source_path !== source_path)
						{
							source_output += this.toFilename(source_path) + ':\n';
							last_source_path = source_path;
						}
						try
						{
							source_output += await this.source(source_path, lineno, 1);
							has_stmt = true;
						}
						catch(err)
						{
							has_stmt = false;
						}
						if(source_output && (last_source_output != source_output))
						{
							output += tab.flush();
							output += source_output;
							last_source_output = source_output;
						}
						const next_stmt = findStatement(addr, { scope: 'next-stmt-within-function' });
						if(next_stmt !== undefined) next_stmt_addr = next_stmt.address;
					}
				}
				
				if(!flag_n || !has_stmt)
				{
					if(print_cursor) tab.write((options.enable_cursor && (pc === addr)) ? '=>' : '  ');
					tab.write(this.toHexAddress(addr));
					tab.write('<' + this.toFunctionNamePlusOffset(addr) + '>:');
				}
				
				const disasm = this.current_processor.disasm(addr, info);
				
				if(!flag_n || !has_stmt)
				{
					if(flag_r || flag_b)
					{
						let hex_encoding = '';
						const buf = this.current_processor.readMemory(addr, info.nextAddr - addr);
						if(flag_r)
						{
							if(this.settings.endian.value === 'little') buf.reverse();
							let encoding = 0n;
							for(let byte of buf)
							{
								encoding = (encoding << 8n) | BigInt(byte);
							}
							hex_encoding += ' ' + Dbg.toHex(encoding);
						}
						else
						{
							for(let byte of buf)
							{
								hex_encoding += ' ' + Dbg.toHex(byte, 2);
							}
						}
						tab.write(hex_encoding);
					}
					
					tab.write(disasm);
				}
				
				addr = BigInt((flag_n && has_stmt & (next_stmt_addr !== undefined)) ? next_stmt_addr : info.nextAddr);
			}
			
			output += tab.flush();
			resolve(output);
		});
	}
	
	/** Disassemble function (asynchronously)
	 * 
	 * @param {number|bigint|string} start - a starting point (an address, a function name, or a register name)
	 * @param {string } [flags] - flags (combination of character set 'srbn')
	 * @return {Promise.<string>} a promise
	 */
	disassemble_function(start, flags, options)
	{
		return new Promise(async (resolve, reject) =>
		{
			let sym;
			if(start !== undefined)
			{
				if((typeof start === 'number') || (typeof start === 'bigint'))
				{
					sym = findSymbol(start, 'function');
					if(sym === undefined)
					{
						reject(new Error('No function contains specified address.'));
						return;
					}
				}
				else if(typeof start === 'string')
				{
					sym = findSymbol(start, 'function');
					if(sym === undefined)
					{
						const m = start.match(/^\s*\$([^\s]+)\s*$/);
						if((m !== null) && (m.length >= 2))
						{
							const reg_name = m[1];
							if(this.current_processor.registers.hasOwnProperty(reg_name))
							{
								const reg = this.current_processor.registers[reg_name];
								sym = findSymbol(reg.get(), 'function');
								if(sym === undefined)
								{
									reject(new Error('No function contains specified address.'));
									return;
								}
							}
							else
							{
								reject(new Error('No register ' + reg_name));
								return;
							}
						}
						else
						{
							reject(new Error('No symbol loaded for ' + start));
							return;
						}
					}
				}
			}
			else
			{
				const pc = Dbg.get_pc(this.current_processor);
				sym = findSymbol(pc, 'function');
				if(sym === undefined)
				{
					reject(new Error('No function contains specified address.'));
					return;
				}
			}
			
			try
			{
				resolve(await this.disassemble_address_range(BigInt(sym.address), BigInt(sym.address) + BigInt(sym.size), flags, { ...options, message: 'Dump of assembler code for function ' + this.toFunctionName(BigInt(sym.address)) + ':' }));
			}
			catch(err)
			{
				reject(err);
				return;
			}
		});
	}
	
	/** Disassemble next line
	 * 
	 * @return {Promise.<string>} a promise
	 */
	disassemble_next_line()
	{
		const disassemble_next_line = this.settings['disassemble-next-line'].value;
		if(disassemble_next_line !== 'off')
		{
			const pc = Dbg.get_pc(this.current_processor);
			const stmt = findStatement(pc, { scope: 'nearest-lower-or-equal-stmt-within-function' });
			const next_stmt = findStatement(pc, { scope: 'next-stmt-within-function' });
			if((stmt !== undefined) && (next_stmt !== undefined) && (stmt.address < next_stmt.address))
			{
				const start = BigInt(stmt.address);
				const end = BigInt(next_stmt.address);
				return this.disassemble_address_range(start, end, (disassemble_next_line === 'auto') ? 'n' : 's', { message: '', enable_cursor: true })
			}
			else
			{
				return this.disassemble_address_range(pc, pc + 1n, (disassemble_next_line === 'auto') ? 'n' : 's', { message: '', enable_cursor: true })
			}
		}
		return Promise.resolve('');
	}
	
	/** Display some informations about events
	 * 
	 * @param {string} section - 'breakpoints' or 'watchpoints'
	 * @return {Promise.<string>} a promise
	 */
	info_events(section)
	{
		return new Promise((resolve, reject) =>
		{
			switch(section)
			{
				case 'breakpoints':
				case 'watchpoints':
				{
					let output = '';
					this[section].forEach((event) =>
					{
						output += event.toString() + ' (' + (event.enable ? 'enabled' : 'disabled') + ')\n';
						if(event instanceof Breakpoint) this.x_addr = event.address;
						else if(event instanceof SubProgramBreakpoint) this.x_addr = event.subprogram.address;
					});
					resolve(output);
					break;
				}
			}
			reject(new Error('No information for ' + section));
		});
	}
	
	/** Display some informations about threads
	 * 
	 * @return {Promise.<string>} a promise
	 */
	info_threads()
	{
		return new Promise((resolve, reject) =>
		{
			let output = '';
			for(let processor of processors)
			{
				let pc = Dbg.get_pc(processor);
				output += ((processor === this.current_processor) ? '*' : ' ') + ' Processor #' + processor.id + ' at ' + this.toHexAddress(pc);
				const sym = findSymbol(pc, 'function');
				if(sym !== undefined) output += ' in <' + sym.name + '>';
				output += '\n';
			}
			resolve(output);
		});
	}
	
	/** Display some informations about inferiors
	 * 
	 * @param {number} [id] - ID of inferior
	 * @return {Promise.<string>} a promise
	 */
	info_inferiors(id)
	{
		return new Promise((resolve, reject) =>
		{
			let output = '';
			for(let exec_bin_file of getExecutableBinaryFiles())
			{
				if((id === undefined) || (id === exec_bin_file.id))
				{
					output += ((exec_bin_file === this.current_exec_bin_file) ? '*' : ' ') + ' Inferior #' + exec_bin_file.id + ': ' + this.toFilename(exec_bin_file.file) + '\n';
				}
			}
			resolve(output);
		});
	}
	
	/** Display some informations about current inferior
	 * 
	 * @return {Promise.<string>} a promise
	 */
	info_current_inferior()
	{
		return new Promise((resolve, reject) =>
		{
			let output = 'Inferior #' + this.current_exec_bin_file.id + ': ' + this.toFilename(this.current_exec_bin_file.file) + '\n';
			resolve(output);
		});
	}
	
	/** Display some informations about all registers
	 * 
	 * @return {Promise.<string>} a promise
	 */
	info_all_registers()
	{
		return new Promise((resolve, reject) =>
		{
			let tab = new Tabular({ columns: 2, border: this.tabular_border });
			const registers = this.current_processor.registers;
			const reg_names = Object.keys(registers);
			for(let i = 0; i < reg_names.length; ++i)
			{
				const reg_name = reg_names[i];
				const reg = registers[reg_name];
				const reg_value = reg.get();
				tab.write(reg_name);
				tab.write('0x' + Dbg.toHex(reg.get(), reg.size * 2));
			}
			resolve(tab.flush());
		});
	}
	
	/** Display some informations about frames
	 * 
	 * @param {number} [frame_id] - frame ID
	 * @return {Promise.<string>} a promise
	 */
	info_frame(frame_id)
	{
		return new Promise((resolve, reject) =>
		{
			let output = '';
			const stack_frame_infos = this.current_processor.getStackFrameInfos((frame_id !== undefined) ? (frame_id + 1) : 0);
			const start = frame_id || 0;
			const end = (frame_id !== undefined) ? (frame_id + 1) : stack_frame_infos.length;
			for(let i = start; i < end; ++i)
			{
				const stack_frame_info = stack_frame_infos[i];
				output += 'Frame #' + stack_frame_info.id + '\n';
				output += '  - Address: ' + this.toHexAddress(stack_frame_info.address) + '\n';
				output += '  - PC: ' + this.toHexAddress(stack_frame_info.pc) + '\n';
				output += '  - Return address: ' + this.toHexAddress(stack_frame_info.returnAddress) + '\n';
			}
			
			resolve(output);
		});
	}
	
	/** Display some informations about selected frame
	 * 
	 * @return {Promise.<string>} a promise
	 */
	info_selected_frame()
	{
		return this.info_frame(this.current_processor.getSelectedStackFrameNumber());
	}
	
	/** Set a breakpoint
	 * 
	 * @param {string|bigint} loc - a location (e.g. 'main.c:1243' or 'main' or '0x1234')
	 */
	breakAt(loc)
	{
		let brkp;
		
		if(typeof loc === 'string')
		{
			let subprogram = findSubProgram(loc);
			if(subprogram !== undefined)
			{
				brkp = new SubProgramBreakpoint(this.current_processor, subprogram);
			}
			else
			{
				let sym = findSymbol(loc, 'function');
				if(sym !== undefined)
				{
					brkp = new Breakpoint(this.current_processor, sym.address);
				}
				else
				{
					try
					{
						brkp = new SourceCodeBreakpoint(this.current_processor, loc, '');
					}
					catch(err)
					{
						console.error(err.message);
					}
				}
			}
		}
		else if(typeof loc === 'bigint')
		{
			brkp = new Breakpoint(this.current_processor, loc);
		}
		
		if(brkp !== undefined)
		{
			try
			{
				brkp.enable = true;
				this.breakpoints.push(brkp);
				this.log(brkp.toString());
			}
			catch(err)
			{
				this.log('Can\'t set breakpoint at ' + loc + ' (' + err.message + ')');
			}
		}
		else
		{
			this.log('Invalid breakpoint location ' + loc);
		}
	}
	
	/** Set a watchpoint
	 * 
	 * @param {string} loc - a data object symbol name
	 */
	watch(loc)
	{
		let wp;
		let sym = findSymbol(loc, 'data-object');
		if(sym !== undefined)
		{
			wp = new Watchpoint(this.current_processor, sym.address, sym.size);
		}
		
		if(wp !== undefined)
		{
			try
			{
				wp.enable = true;
				this.watchpoints.push(wp);
				this.log(wp.toString());
			}
			catch(err)
			{
				this.log('Can\'t set watchpoint at ' + loc + ' (' + err.message + ')');
			}
		}
		else
		{
			this.log('Invalid watchpoint location \'' + loc + '\'');
		}
	}
	
	/** Set a watchpoint at an address range
	 * 
	 * @param {bigint} start - a starting address
	 * @param {bigint} end - an end address (excluded)
	 */
	watch_address_range(start, end)
	{
		if(start < end)
		{
			let wp = new Watchpoint(this.current_processor, start, end - start);
			try
			{
				wp.enable = true;
				this.watchpoints.push(wp);
				this.log(wp.toString());
			}
			catch(err)
			{
				this.log('Can\'t set watchpoint at [' + this.toHexAddress(start) + ', ' + this.toHexAddress(end) + '[ (' + err.message + ')');
			}
		}
		else
		{
			this.log('Invalid watchpoint address range');
		}
	}
	
	/** Enable/Disable a breakpoint or watchpoint
	 * 
	 * @param {number} id - id of breakpoint or watchpoint
	 * @param {boolean} flag - enable: true, disable: false
	 */
	enable(id, flag)
	{
		if(typeof id !== 'number') throw Error(id + ' is not a number');
		[ this.breakpoints, this.watchpoints ].forEach((list) =>
		{
			let event = list.find((event) => event.id === id);
			if(event !== undefined) event.enable = flag;
		});
	}
	
	/** Delete a breakpoint or watchpoint
	 * 
	 * @param {number} id - id of breakpoint or watchpoint
	 */
	deleteEvent(id)
	{
		if(typeof id !== 'number') throw Error(id + ' is not a number');
		let brkp = this.breakpoints.find((brkp) => brkp.id === id);
		if(brkp !== undefined)
		{
			brkp.enable = false; 
			this.breakpoints = this.breakpoints.filter((brkp) => brkp.id !== id);
		}
		else
		{
			let wp = this.watchpoints.find((wp) => wp.id === id);
			if(wp !== undefined)
			{
				wp.enable = false; 
				this.watchpoints = this.watchpoints.filter((wp) => wp.id !== id);
			}
		}
	}
	
	/** Repeat asynchronously until count or an event occurs
	 *
	 * @param {Dbg} obj - object
	 * @param {function} async_method - asynchronous method of object to repeatedly call
	 * @param {string} op_name - operation name
	 * @param {number} [count] - repeat count
	 * @return {Promise} a promise
	 */ 
	repeat(obj, async_method, op_name, count = 1)
	{
		return new Promise(async (resolve, reject) =>
		{
			if(count)
			{
				do
				{
					const promises = [ async_method.call(obj, { unblock: true }), continueExecution() ]; 
					const promise_race_indexed = (promises) => Promise.race(promises.map((promise, idx) => promise.then((result) => [result, idx])));
					
					try
					{
						const [processor, idx] = await promise_race_indexed(promises);
						if(processor !== undefined) this.current_processor = processor;
						if(idx != 0) break; // continueExecution promise resolved?
					}
					catch(err)
					{
						reject('"' + op_name + '" failed (' + err + ')');
						return;
					}
				}
				while(--count);
				
				const disassemble_next_line = this.settings['disassemble-next-line'].value;
				if(disassemble_next_line !== 'off')
				{
					try
					{
						this.output.write(await this.disassemble_next_line());
					}
					catch(err)
					{
						reject('disassemble failed (' + err + ')');
						return;
					}
				}
			}
			
			resolve();
		});
	}
	
	/** asynchronous "stepi"
	 * 
	 * @param {number} [count] - repeat count
	 * @return {Promise} a promise
	 */
	stepi(count = 1) { return this.repeat(this.current_processor, this.current_processor.stepInstruction, 'stepi', count); }
	
	/** asynchronous "step"
	 * 
	 * @param {number} [count] - repeat count
	 * @return {Promise} a promise
	 */
	step(count = 1) { return this.repeat(this.current_processor, this.current_processor.step, 'step', count); }
	
	/** asynchronous "nexti"
	 * 
	 * @param {number} [count] - repeat count
	 * @return {Promise} a promise
	 */
	nexti(count = 1) { return this.repeat(this.current_processor, this.current_processor.nextInstruction, 'nexti', count); }
	
	/** asynchronous "next"
	 * 
	 * @param {number} [count] - repeat count
	 * @return {Promise} a promise
	 */
	next(count = 1) { return this.repeat(this.current_processor, this.current_processor.next, 'next', count); }
	
	/** asynchronous "finish"
	 * 
	 * @return {Promise} a promise
	*/
	finish()
	{
		return this.repeat(this.current_processor, this.current_processor.finish, 'finish', 1);
	}
	
	/** Return from current function
	 * 
	 * @param {*} return_value - the return value
	 */
	returnFromFunction(return_value)
	{
		try
		{
			this.current_processor.returnFromFunction(return_value);
		}
		catch(err)
		{
			this.error('Can\' return from current function (' + err.message + ')');
		}
	}
	
	/** Continue (asynchronously) execution until any event occurs
	 * 
	 * @return {Promise} a promise
	 */
	continueExecution()
	{
		return new Promise(async (resolve, reject) =>
		{
			continueExecution({ unblock: true }).then(async (processor) =>
			{
				if(processor !== undefined) this.current_processor = processor;
				const disassemble_next_line = this.settings['disassemble-next-line'].value;
				if(disassemble_next_line !== 'off')
				{
					try
					{
						this.output.write(await this.disassemble_next_line());
					}
					catch(err)
					{
						reject('disassemble failed (' + err + ')');
						return;
					}
				}
				resolve();
			}).catch((err) =>
			{
				reject('"continue" failed (' + err.message + ')');
			});
		});
	}
	
	/** Build (asynchronously) backtrace
	 * 
	 * @param {number} maxStackFrames - maximum depth
	 * @return {Promise.<string>} a promise
	 */
	backtrace(maxStackFrames)
	{
		return new Promise((resolve, reject) =>
		{
			let output = '';
			const stack_frame_infos = this.current_processor.getStackFrameInfos(maxStackFrames);
			for(let i = 0; i < stack_frame_infos.length; ++i)
			{
				const stack_frame_info = stack_frame_infos[i];
				output += '#' + stack_frame_info.id;
				if(i != 0) output += ' ' + this.toHexAddress(BigInt(stack_frame_info.pc));
				const display_addr = (i === 0) ? BigInt(stack_frame_info.pc) : (BigInt(stack_frame_info.pc) - 1n);
				if(i != 0) output += ' in';
				output += ' ' + this.toFunctionName(display_addr);
				const stmt = findStatement(display_addr, { scope: 'nearest-lower-or-equal-stmt-within-function' });
				if(stmt !== undefined)
				{
					const lineno = stmt.lineno;
					const source_path = stmt.sourceDirname ? (stmt.sourceDirname + '/' + stmt.sourceFilename) : stmt.sourceFilename;
					output += ' at ';
					output += this.toFilename(source_path) + ':' + lineno;
				}
				output += '\n';
			}
			
			resolve(output);
		});
	}
	
	/** Select current stack frame
	 * 
	 * @param {number} frame_id - ID of stack frame
	 */
	select_frame(frame_id)
	{
		try
		{
			this.current_processor.selectStackFrame(frame_id);
		}
		catch(err)
		{
			this.output.write(err.message + '\n');
		}
	}
	
	/** Log a message
	 * 
	 * @param {string} msg - some text
	 */
	log(msg)
	{
		this.output.write(msg += '\n');
	}
	
	/** Print an error message
	 * 
	 * @param {string} msg - some text
	 */
	error(msg)
	{
		process.stderr.write(msg += '\n');
	}
	
	/** Complete function name
	 * 
	 * @param {string} name - seed of name
	 * @return {Array.<string>} possible function names
	 */
	complete_functions(name)
	{
		return getSymbols().filter((sym) => (sym.type === 'function') && sym.name.startsWith(name)).map((sym) => sym.name);
	}
		
	/** Complete data object name
	 * 
	 * @param {string} name - seed of name
	 * @return {Array.<string>} possible data object names
	 */
	complete_data_objects(name)
	{
		return this.current_processor.getDataObjectNames().filter((data_object_name) => data_object_name.startsWith(name));
	}
	
	/** Complete register name
	 * 
	 * @param {string} name - seed of name
	 * @return {Array.<string>} possible register names
	 */
	complete_registers(name)
	{
		if(!name || name.startsWith('$'))
		{
			const lower_case_name = name.substring(1).toLowerCase();
			return Object.values(this.current_processor.registers).filter((reg) => reg.name.toLowerCase().startsWith(lower_case_name)).map((reg) => '$' + reg.name);
		}
		return [];
	}
	
	/** Add help about settings to completion rules for REPL
	 * 
	 * @param {boolean} enum_values - Whether to enumerate possible values for setting
	 * @param {Array.<string>} prefix - prefix completion rule (to reach setting) of setting
	 * @param {Object} settings - an object of setting hierarchy
	 */
	add_help_settings_completion_rules(enum_values, prefix, settings)
	{
		if(prefix === undefined) prefix = [];
		if(settings === undefined) settings = this.settings;
		const keys = Object.keys(settings);
		for(let i = 0; i < keys.length; ++i)
		{
			const key = keys[i];
			const setting = settings[key];
			if((typeof setting === 'object') && !Array.isArray(setting))
			{
				const new_prefix = [ ...prefix, key ];
				if(setting.hasOwnProperty('value'))
				{
					this.completion_rules.push(new_prefix);
					if(enum_values && setting.hasOwnProperty('enum'))
					{
						for(let value of setting.enum)
						{
							this.completion_rules.push([ ...new_prefix, value.toString() ]);
						}
					}
				}
				else
				{
					this.add_help_settings_completion_rules(enum_values, new_prefix, setting);
				}
			}
		}
	}
	
	/** Add help completion rules for REPL
	 * 
	 * @param {Object} settings - an object of setting hierarchy
	 * @param {Array.<string>} prefix - prefix completion rule (to reach help) of help
	 */
	add_help_completion_rules(help, prefix)
	{
		if(prefix === undefined) prefix = [ 'help' ];
		const keys = Object.keys(help);
		for(let i = 0; i < keys.length; ++i)
		{
			const key = keys[i];
			if(key !== '_')
			{
				const child_help = help[key];
				if((typeof child_help === 'object') && !Array.isArray(child_help))
				{
					const new_prefix = [ ...prefix, key ];
					if(child_help.hasOwnProperty('_')) this.completion_rules.push(new_prefix);
					this.add_help_completion_rules(child_help, new_prefix);
				}
				else if(typeof child_help === 'string')
				{
					this.completion_rules.push([ ...prefix, key ]);
				}
			}
		}
	}
	
	/** Completion function
	 * 
	 * @param {string} text - original string
	 * @param {number} [start] - lower bound of the region of text to complete
	 * @param {number} [end] - upper bound of the region of text to complete
	 * @return {Array.<string>} the completions
	 */
	complete(text, start, end)
	{
		let word;
		if(start !== undefined) word = text.substring(0, start).split(/\s+/).length - 1;
		let completions = new Set();
		const tokens = ((end !== undefined) ? text.substring(0, end) : text).split(/\s+/);
		for(let i = 0; i < this.completion_rules.length; ++i)
		{
			const command = this.completion_rules[i];
			let prefix = '';
			for(let j = 0; (j < command.length) && (j < tokens.length); ++j)
			{
				const token = tokens[j];
				const key = command[j];
				if(typeof key === 'string')
				{
					if(j === (tokens.length - 1))
					{
						if(key.startsWith(token))
						{
							if((word === undefined) || (j === word))
							{
								const completion = prefix + key;
								completions.add(completion);
							}
						}
					}
					else if(token !== key)
					{
						break;
					}
				}
				else if(typeof key === 'function')
				{
					const keys = key(token);
					if((j === (tokens.length - 1)) && ((word === undefined) || (j === word)))
					{
						keys.forEach((k) => completions.add(prefix + k));
					}
					else if(keys.find((k) => k === token) === undefined)
					{
						break;
					}
				}
				
				if(word === undefined) prefix += token + ' ';
			}
		}
		
		return [ ...completions ];
	}
	
	/** Utility function to escape quotes and embed encompassing quotes within a string
	 *  undefined -> undefined
	 *  'abc"d"' -> '"abc\\"d\\""'
	 * 
	 * @param {string} s - a string
	 * @return {string} a string
	 */
	static str(s)
	{
		return (s === undefined) ? undefined : ('"' + s.replaceAll(/\"/g, '\\"') + '"');
	}
	
	/** Preprocess REPL commands
	 * 
	 * @param {string} cmd - command
	 * @return {string} a preprocessed command
	 */
	preprocess(cmd)
	{
		cmd = cmd.replace(/\r?\n$/, ''); // strip last newline characters
		if((this.last_cmd !== undefined) && (cmd.length === 0)) return this.last_cmd;
		this.last_cmd = undefined;
		let m;
		if(((m = cmd.match(/^(si|stepi)\s+(\d+)\s*$/                                                                              )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.stepi(' + m[2] + ')\n';
		if(((m = cmd.match(/^(si|stepi)\s*$/                                                                                      )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.stepi()\n';
		if(((m = cmd.match(/^(s|step)\s+(\d+)\s*$/                                                                                )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.step(' + m[2] + ')\n';
		if(((m = cmd.match(/^(s|step)\s*$/                                                                                        )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.step()\n';
		if(((m = cmd.match(/^(ni|nexti)\s+(\d+)\s*$/                                                                              )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.nexti(' + m[2] + ')\n';
		if(((m = cmd.match(/^(ni|nexti)\s*$/                                                                                      )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.nexti()\n';
		if(((m = cmd.match(/^(n|next)\s+(\d+)\s*$/                                                                                )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.next(' + m[2] + ')\n';
		if(((m = cmd.match(/^(n|next)\s*$/                                                                                        )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.next()\n';
		if(((m = cmd.match(/^(fin|finish)\s*$/                                                                                    )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.finish()\n';
		if(((m = cmd.match(/^(return)\s+(.*)$/                                                                                    )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.returnFromFunction(' + m[2] + ')\n';
		if(((m = cmd.match(/^(return)\s*$/                                                                                        )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.returnFromFunction()\n';
		if(((m = cmd.match(/^(c|cont|continue)\s*$/                                                                               )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.continueExecution()\n';
		if(((m = cmd.match(/^(q|quit)\s*$/                                                                                        )) !== null) && (m.length >= 2)) return /* once */      'quit()\n';
		if(((m = cmd.match(/^(b|break)\s+\*(0x[0-9a-fA-F]+|\d+)\s*$/                                                              )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.breakAt(Dbg.user_addr(' + Dbg.str(m[2]) + '))\n';
		if(((m = cmd.match(/^(b|break)\s+(.*)$/                                                                                   )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.breakAt(' + Dbg.str(m[2]) + ')\n';
		if(((m = cmd.match(/^(w|watch)\s+((0x[0-9a-fA-F]+|\d+)\s*,\s*(0x[0-9a-fA-F]+|\d+))\s*$/                                   )) !== null) && (m.length >= 5)) return this.last_cmd = 'Dbg.instance.watch_address_range(Dbg.user_addr(' + Dbg.str(m[3]) + '),Dbg.user_addr(' +  Dbg.str(m[4]) + '))\n';
		if(((m = cmd.match(/^(w|watch)\s+((0x[0-9a-fA-F]+|\d+)\s*,\s*\+(0x[0-9a-fA-F]+|\d+))\s*$/                                 )) !== null) && (m.length >= 5)) return this.last_cmd = 'Dbg.instance.watch_address_range(Dbg.user_addr(' + Dbg.str(m[3]) + '),Dbg.user_addr(' + Dbg.str(m[3]) + ') + Dbg.user_addr(' +  Dbg.str(m[4]) + '))\n';
		if(((m = cmd.match(/^(w|watch)\s+(.*)$/                                                                                   )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.watch(' + Dbg.str(m[2]) + ')\n';
		if(((m = cmd.match(/^(info)\s+(breakpoints)\s*$/                                                                          )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_events(' + Dbg.str(m[2]) + '))\n';
		if(((m = cmd.match(/^(info)\s+(watchpoints)\s*$/                                                                          )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_events(' + Dbg.str(m[2]) + '))\n';
		if(((m = cmd.match(/^(info)\s+(threads)\s*$/                                                                              )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_threads())\n';
		if(((m = cmd.match(/^(info)\s+(inferiors)\s+(\d+)$/                                                                       )) !== null) && (m.length >= 4)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_inferiors(' + m[3] + '))\n';
		if(((m = cmd.match(/^(info)\s+(inferiors)\s*$/                                                                            )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_inferiors())\n';
		if(((m = cmd.match(/^(info)\s+(all-registers)\s*$/                                                                        )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_all_registers())\n';
		if(((m = cmd.match(/^(info)\s+(frame|f)\s+(\d+)\s*$/                                                                      )) !== null) && (m.length >= 4)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_frame(' + m[3] + '))\n';
		if(((m = cmd.match(/^(info)\s+(frame|f)\s*$/                                                                              )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_frame())\n';
		if(((m = cmd.match(/^(del|delete)\s+(\d+)\s*$/                                                                            )) !== null) && (m.length >= 3)) return /* once */      'Dbg.instance.deleteEvent(' + m[2] + ')\n';
		if(((m = cmd.match(/^(enable)\s+(\d+)\s*$/                                                                                )) !== null) && (m.length >= 3)) return /* once */      'Dbg.instance.enable(' + m[2] + ', true)\n';
		if(((m = cmd.match(/^(disable)\s+(\d+)\s*$/                                                                               )) !== null) && (m.length >= 3)) return /* once */      'Dbg.instance.enable(' + m[2] + ', false)\n';
		if(((m = cmd.match(/^(bt|backtrace)\s*$/                                                                                  )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.backtrace())\n';
		if(((m = cmd.match(/^(thread)\s+(\d+)\s*$/                                                                                )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.thread(' + m[2] + ')\n';
		if(((m = cmd.match(/^(inferior)\s+(\d+)\s*$/                                                                              )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.inferior(' + m[2] + ')\n';
		if(((m = cmd.match(/^(inferior)\s*$/                                                                                      )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_current_inferior())\n';
		if(((m = cmd.match(/^(x)\s*(\/(-?\d+)?([xzduotacfsi])?([bhwg])?)?\s+(0x[0-9a-fA-F]+|\d+)\s*$/                             )) !== null) && (m.length >= 7)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.examine_memory(Dbg.user_addr(' + Dbg.str(m[6]) + '),' +  Dbg.str(m[3]) + ',' + Dbg.str(m[4]) + ',' + Dbg.str(m[5]) + '))\n';
		if(((m = cmd.match(/^(x)\s*(\/(-?\d+)?([xzduotacfsi])?([bhwg])?)?\s*$/                                                    )) !== null) && (m.length >= 6)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.examine_memory(undefined,' + Dbg.str(m[3]) + ',' + Dbg.str(m[4]) + ',' + Dbg.str(m[5]) + '))\n';
		if(((m = cmd.match(/^(x)\s*(\/(-?\d+)?([xzduotacfsi])?([bhwg])?)?\s+(.*)$/                                                )) !== null) && (m.length >= 7)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.examine_memory(' + Dbg.str(m[6]) + ',' + Dbg.str(m[3]) + ',' + Dbg.str(m[4]) + ',' + Dbg.str(m[5]) + '))\n';
		if(((m = cmd.match(/^(show)\s+(.*)$/                                                                                      )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.show_setting(' + Dbg.str(m[2]) + ')\n';
		if(((m = cmd.match(/^(set)\s+\$(.*)\s+=\s+(0x[0-9a-fA-F]+|\d+)$/                                                          )) !== null) && (m.length >= 4)) return this.last_cmd = 'Dbg.instance.set_register(' + Dbg.str(m[2]) + ',BigInt(' + Dbg.str(m[3]) + '))\n';
		if(((m = cmd.match(/^(set)\s+((var|variable)\s+)?(([^=]+)\s+=\s+(.*))$/                                                   )) !== null) && (m.length >= 7)) return this.last_cmd = 'Dbg.instance.set_variable(' + Dbg.str(m[5]) + ',' + m[6] + ')\n';
		if(((m = cmd.match(/^(set)\s+(.*)\s+([^\s]+)\s*$/                                                                         )) !== null) && (m.length >= 4)) return this.last_cmd = 'Dbg.instance.set_setting(' + Dbg.str(m[2]) + ',' + Dbg.str(m[3]) + ')\n';
		if(((m = cmd.match(/^(print)\s*(\/([xzduotacfs]))?\s+(.*)$/                                                               )) !== null) && (m.length >= 5)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.print(' + Dbg.str(m[4]) + ',' + Dbg.str(m[3]) + '))\n';
		if(((m = cmd.match(/^(disas|disass|disassemble)\s*(\/([srb]+))?\s+((0x[0-9a-fA-F]+|\d+)\s*,\s*(0x[0-9a-fA-F]+|\d+))\s*$/  )) !== null) && (m.length >= 7)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.disassemble_address_range(Dbg.user_addr(' + Dbg.str(m[5]) + '),Dbg.user_addr(' + Dbg.str(m[6]) + '),' + Dbg.str(m[3]) + ',{enable_cursor: true}))\n';
		if(((m = cmd.match(/^(disas|disass|disassemble)\s*(\/([srb]+))?\s+((0x[0-9a-fA-F]+|\d+)\s*,\s*\+(0x[0-9a-fA-F]+|\d+))\s*$/)) !== null) && (m.length >= 7)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.disassemble_address_range(Dbg.user_addr(' + Dbg.str(m[5]) + '),Dbg.user_addr(' + Dbg.str(m[5]) + ')+Dbg.user_addr(' + Dbg.str(m[6]) + '),' + Dbg.str(m[3]) + ',{enable_cursor: true}))\n';
		if(((m = cmd.match(/^(disas|disass|disassemble)\s*(\/([srb]+))?\s+(0x[0-9a-fA-F]+|\d+)\s*$/                               )) !== null) && (m.length >= 5)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.disassemble_function(Dbg.user_addr(' + Dbg.str(m[4]) + '),' + Dbg.str(m[3]) + ',{enable_cursor: true}))\n';
		if(((m = cmd.match(/^(disas|disass|disassemble)\s*(\/([srb]+))?\s*$/                                                      )) !== null) && (m.length >= 4)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.disassemble_function(undefined,' + Dbg.str(m[3]) + ',{enable_cursor: true}))\n';
		if(((m = cmd.match(/^(disas|disass|disassemble)\s*(\/([srb]+))?\s+(.*)$/                                                  )) !== null) && (m.length >= 5)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.disassemble_function(' + Dbg.str(m[4]) + ',' + Dbg.str(m[3]) + ',{enable_cursor: true}))\n';
		if(((m = cmd.match(/^(symbol-file)\s+(.*)$/                                                                               )) !== null) && (m.length >= 3)) return /* once */      'Dbg.instance.symbol_file(' + Dbg.str(m[2]) + ')\n';
		if(((m = cmd.match(/^(directory|dir)\s+(.*)$/                                                                             )) !== null) && (m.length >= 3)) return /* once */      'Dbg.instance.directory(' + Dbg.str(m[2]) + ')\n';
		if(((m = cmd.match(/^(directory|dir)\s*$/                                                                                 )) !== null) && (m.length >= 2)) return /* once */      'Dbg.instance.directory()\n';
		if(((m = cmd.match(/^(help)\s+(.*)$/                                                                                      )) !== null) && (m.length >= 3)) return /* once */      'Dbg.instance.show_help(' + Dbg.str(m[2]) + ')\n';
		if(((m = cmd.match(/^(help)\s*$/                                                                                          )) !== null) && (m.length >= 2)) return /* once */      'Dbg.instance.show_help()\n';
		if(((m = cmd.match(/^(list|l)\s+\*(0x[0-9a-fA-F]+|\d+)\s*$/                                                               )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.list(Dbg.user_addr(' + Dbg.str(m[2]) + ')))\n';
		if(((m = cmd.match(/^(list|l)\s+(\+|-|\.)\s*$/                                                                            )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.list(' + Dbg.str(m[2]) + '))\n';
		if(((m = cmd.match(/^(list|l)\s*$/                                                                                        )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.list())\n';
		if(((m = cmd.match(/^(list|l)\s+(.*)$/                                                                                    )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.list(' + Dbg.str(m[2]) + '))\n';
		if(((m = cmd.match(/^(frame|f)\s+(\d+)\s*$/                                                                               )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.select_frame(' + m[2] + ')\n';
		if(((m = cmd.match(/^(frame|f)\s*$/                                                                                       )) !== null) && (m.length >= 2)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.info_selected_frame())\n';
		if(((m = cmd.match(/^(whatis)\s+(.*)$/                                                                                    )) !== null) && (m.length >= 3)) return this.last_cmd = 'Dbg.instance.paginate(Dbg.instance.whatis(' + Dbg.str(m[2]) + '))\n';
		return undefined;
	}
}

module.exports = Dbg;
