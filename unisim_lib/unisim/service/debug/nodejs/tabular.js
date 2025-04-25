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

class Tabular
{
	constructor(options = {})
	{
		this.options = options;
		this.clear();
	}
	
	clear()
	{
		this.row = 0;
		this.col = 0;
		this.tab = [];
	}
	
	write(...args)
	{
		args.forEach((text) =>
		{
			if(this.row >= this.tab.length) this.tab.push([]);
			let row = this.tab[this.row];
			row.push(text);
			if(++this.col >= (this.options.columns || 1))
			{
				++this.row;
				this.col = 0;
			}
		});
		return this;
	}
	
	flush()
	{
		let output = '';
		
		const drop_ansi_codes = (text) => text.replace(/[\u001b\u009b][[()#;?]*(?:[0-9]{1,4}(?:;[0-9]{0,4})*)?[0-9A-ORZcf-nqry=><]/g,'');
		
		if(this.tab.length > 0)
		{
			let col_widths = [];
			for(let i = 0; i < this.tab.length; ++i)
			{
				const row = this.tab[i];
				while(col_widths.length < row.length) col_widths.push(0);
				for(let j = 0; j < row.length; ++j)
				{
					const text = row[j].toString().replace(/\t/g, ' '.repeat(this.options.tab_size || 8));
					const bare_text = drop_ansi_codes(text);
					if(bare_text.length > col_widths[j]) col_widths[j] = bare_text.length;
				}
			}
			for(let i = 0; i < this.tab.length; ++i)
			{
				const row = this.tab[i];
				if(Array.isArray(row))
				{
					for(let j = 0; j < row.length; ++j)
					{
						const text = row[j].toString().replace(/\t/g, ' '.repeat(this.options.tab_size || 8));
						const bare_text = drop_ansi_codes(text);
						output += text.padEnd(col_widths[j] + (text.length - bare_text.length), ' ');
						output += ' '.repeat(this.options.border || 1);
					}
					output += '\n';
				}
			}
		}
		this.clear();
		return output;
	}
}

module.exports = Tabular;
