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

const Dbg = require(__dirname + '/dbg.js');

class REPL
{
	static instance = new REPL();
	static default_completer;
	
	static start()
	{
		// get default completer of Node.js REPL
		const repl = require('node:repl');
		const fs = require('node:fs');
		const tmp_repl_server = repl.start(
		{
			input: fs.createReadStream("/dev/null"),
			output: fs.createWriteStream("/dev/null"),
		});
		REPL.default_completer = tmp_repl_server.completer;
		tmp_repl_server.close();
		
		REPL.instance.start();
	}
	
	constructor()
	{
		this.debug = false;
	}
	
	start()
	{
		process.stdout.write('Welcome to the UNISIM-VP debugger based on Node.js Javascript runtime environment.\n');
		process.stdout.write('This debugger is fully written in Javascript.\n');
		process.stdout.write('JavaScript code or debugger commands can be intertwined.\n');
		process.stdout.write('For help, type "help".\n');
		
		const repl = require('node:repl');
		const vm = require('node:vm');
	
		const repl_server_options =
		{
				prompt: 'debugger> ',
				eval: this.evaluate.bind(this),
				completer: this.completer.bind(this),
				useGlobal: true
		};
		this.repl_server = repl.start(repl_server_options);
		this.repl_server.on('exit', () => quit());
	}
	
	async evaluate(cmd, context, filename, callback)
	{
		const preprocessed = Dbg.instance.preprocess(cmd);
		if(this.debug) process.stdout.write((preprocessed || cmd));
		let result = vm.runInThisContext(preprocessed || cmd, { filename });
		if(preprocessed && (result instanceof Promise))
		{
			try
			{
				result = await result;
			}
			catch(err)
			{
				console.error(err);
				result = undefined;
			}
		}
		
		callback(null, result);
	}
	
	/** Completion function (to be used as callback for an existing completer, e.g. the one of Node.js REPL)
	 * 
	 * @param {string} cmd - original line being edited
	 * @param {function} callback - a callback that gets called with completions
	 * @param {Error} err - ????
	 * @param {Array.<string>} results - results from existing completer (e.g. the one of Node.js REPL)
	 */
	complete(cmd, callback, err, results)
	{
		const completions = Dbg.instance.complete(cmd);
		
		if(completions.length > 0)
		{
			if(cmd.indexOf(' ') !== -1)
			{
				callback(null, [ [ ...completions ], cmd ]);
			}
			else
			{
				const combined_results = [ [ ...results[0], ...completions ], cmd ];
				callback(null, combined_results);
			}
		}
		else
		{
			callback(null, results);
		}
	}
	
	completer(cmd, callback)
	{
		REPL.default_completer.call(this.repl_server, cmd, this.complete.bind(this, cmd, callback));
	}
}

if(inBuiltinREPL)
{
	throw new Error('This module shall not be used when in simulator builtin REPL');
}

if(!inInteractiveMode)
{
	throw new Error('This module shall be used when simulator is in interactive mode');
}

setTimeout(function()
{
	REPL.start();
}, 0);
