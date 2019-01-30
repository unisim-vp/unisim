// needs unisim/service/http_server/embedded_script.js

save_instrumenter_scroller_scroll_top = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		sessionStorage.setItem(item_prefix() + 'div.scroller.scrollTop', el.scrollTop);
	}
}

restore_instrumenter_scroller_scroll_top = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		var t = sessionStorage.getItem(item_prefix() + 'div.scroller.scrollTop');
		if(t)
		{
			el.scrollTop = t;
		}
	}
	
	restore_window_scroll_top();
}

save_instrumenter_scroll_top = function()
{
	save_instrumenter_scroller_scroll_top();
	save_window_scroll_top();
}

restore_instrumenter_scroll_top = function()
{
	restore_instrumenter_scroller_scroll_top();
	restore_window_scroll_top();
}

reload_instrumenter_page = function()
{
	save_instrumenter_scroller_scroll_top();
	reload_page();
}

var window_resize_refresh_period = 500;
var min_scroller_height = 76;
var window_inner_width = 0;
var window_inner_height = 0;

resize_content = function(content_width, content_height)
{
	var scroller = document.querySelector('div.scroller');
	if(scroller)
	{
		var rect = scroller.getBoundingClientRect();
		var scroller_height = Math.max(Math.floor(content_height - rect.top - 1), min_scroller_height);
		scroller.style.height = scroller_height + 'px';
	}
}

resize = function()
{
	var new_window_inner_width = window.innerWidth;
	var new_window_inner_height = window.innerHeight;
	if((new_window_inner_width != window_inner_width) || (new_window_inner_height != window_inner_height))
	{
		window_inner_width = new_window_inner_width;
		window_inner_height = new_window_inner_height;
		resize_content(window_inner_width, window_inner_height);
	}
}

document.addEventListener('DOMContentLoaded', function(event) { resize(); restore_instrumenter_scroll_top(); setInterval(resize, window_resize_refresh_period); });
