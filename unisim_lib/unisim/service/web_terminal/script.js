// needs unisim/service/http_server/embedded_script.js

GUI.prototype.form = null;
GUI.prototype.input_key = null;
GUI.prototype.input_ctrl = null;
GUI.prototype.input_shift = null;
GUI.prototype.input_alt = null;
GUI.prototype.input_meta = null;
GUI.prototype.screen_buffer = null;
GUI.prototype.bound_on_keypress = null;
GUI.prototype.bound_on_focus_in = null;
GUI.prototype.bound_on_focus_out = null;

GUI.prototype.on_load = function()
{
// 	console.log('load');
	var target = this.get_next_target();
	this.form = document.createElement('form');
	this.form.setAttribute('action', window.location.href);
	this.form.setAttribute('method', 'post');
	this.form.setAttribute('target', target);
	this.form.setAttribute('style', 'display:none');
	
	this.input_key = document.createElement('input');
	this.input_key.setAttribute('type', 'text');
	this.input_key.setAttribute('name', 'key');
	this.input_key.setAttribute('value', '');

	this.input_ctrl = document.createElement('input');
	this.input_ctrl.setAttribute('type', 'text');
	this.input_ctrl.setAttribute('name', 'ctrl');
	this.input_ctrl.setAttribute('value', '');

	this.input_shift = document.createElement('input');
	this.input_shift.setAttribute('type', 'text');
	this.input_shift.setAttribute('name', 'shift');
	this.input_shift.setAttribute('value', '');

	this.input_alt = document.createElement('input');
	this.input_alt.setAttribute('type', 'text');
	this.input_alt.setAttribute('name', 'alt');
	this.input_alt.setAttribute('value', '');

	this.input_meta = document.createElement('input');
	this.input_meta.setAttribute('type', 'text');
	this.input_meta.setAttribute('name', 'meta');
	this.input_meta.setAttribute('value', '');

	this.form.appendChild(this.input_key);
	this.form.appendChild(this.input_ctrl);
	this.form.appendChild(this.input_shift);
	this.form.appendChild(this.input_alt);
	this.form.appendChild(this.input_meta);
	document.body.appendChild(this.form);

	this.screen_buffer = document.getElementById('screen-buffer');
	if(this.screen_buffer)
	{
		this.bound_on_keypress = function(event) { this.on_keypress(event); }.bind(this);
		this.screen_buffer.addEventListener('keydown', this.bound_on_keypress);
		var focused = !this.within_tiled_gui() || parseInt(sessionStorage.getItem(this.storage_item_prefix() + 'focused'));
		if(focused)
		{
// 			console.log('focus');
			this.screen_buffer.focus();
		}
		
		if(this.within_tiled_gui())
		{
			this.bound_on_focus_in = this.on_focus_in.bind(this);
			this.bound_on_focus_out = this.on_focus_out.bind(this);
			this.screen_buffer.addEventListener('focusin', this.bound_on_focus_in);
			this.screen_buffer.addEventListener('focusout', this.bound_on_focus_out);
		}
		else
		{
			this.bound_on_focus_in = null;
			this.bound_on_focus_out = null;
		}
	}
	else
	{
		this.bound_on_keypress = null;
		this.bound_on_focus_in = null;
		this.bound_on_focus_out = null;
	}
}

GUI.prototype.on_unload = function()
{
// 	console.log('unload');
	if(this.bound_on_focus_in)
	{
		this.screen_buffer.removeEventListener('focusin', this.bound_on_focus_in);
		this.bound_on_focus_in = null
	}
	if(this.bound_on_focus_out)
	{
		this.screen_buffer.removeEventListener('focusout', this.bound_on_focus_out);
		this.bound_on_focus_out = null;
	}
	if(this.screen_buffer)
	{
		this.screen_buffer.blur();
	}
	if(this.bound_on_keypress)
	{
		this.screen_buffer.removeEventListener('keydown', this.bound_on_keypress);
		this.bound_on_keypress = null;
	}
	if(this.form)
	{
		document.body.removeChild(this.form);
		if(this.input_key)
		{
			this.form.removeChild(this.input_key);
		}
		if(this.input_ctrl)
		{
			this.form.removeChild(this.input_ctrl);
		}
		if(this.input_shift)
		{
			this.form.removeChild(this.input_shift);
		}
		if(this.input_alt)
		{
			this.form.removeChild(this.input_alt);
		}
		if(this.input_meta)
		{
			this.form.removeChild(this.input_meta);
		}
	}
	this.input_key = null;
	this.input_ctrl = null;
	this.input_shift = null;
	this.input_alt = null;
	this.input_meta = null;
	this.form = null;
	this.screen_buffer = null;
}

GUI.prototype.on_keypress = function(event)
{
	//console.log('keydown: key=\'' + event.key + '\', ctrlKey=' + event.ctrlKey + ', shiftKey=' + event.shiftKey + ', altKey=' + event.altKey + ', metaKey=' + event.metaKey);
	if(this.form && this.input_key && this.input_ctrl && this.input_shift && this.input_alt && this.input_meta)
	{
		this.input_key.setAttribute('value', event.key);
		this.input_ctrl.setAttribute('value', event.ctrlKey ? '1' : '0');
		this.input_shift.setAttribute('value', event.shiftKey ? '1' : '0');
		this.input_alt.setAttribute('value', event.altKey ? '1' : '0');
		this.input_meta.setAttribute('value', event.metaKey ? '1' : '0');
		this.form.submit();
	}
}

GUI.prototype.on_focus_in = function()
{
// 	console.log('focus in');
	sessionStorage.setItem(this.storage_item_prefix() + 'focused', 1);
}

GUI.prototype.on_focus_out = function()
{
// 	console.log('focus out');
	sessionStorage.setItem(this.storage_item_prefix() + 'focused', 0);
}

var gui = new GUI();
