itemPrefix = function()
{
	return (window.frameElement && window.name) ? (window.name + '.') : '';
}

saveScrollTop = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		sessionStorage.setItem(itemPrefix() + 'div.scroller.scrollTop', el.scrollTop);
	}
	
	var own_tab = null;
	if(parent.gui && (own_tab = parent.gui.find_tab_by_name(window.name)))
	{
		// within tiled GUI
		own_tab.save_scroll_position();
	}
	else
	{
		sessionStorage.setItem(itemPrefix() + 'window.scrollY', window.scrollY);
		sessionStorage.setItem(itemPrefix() + 'window.scrollX', window.scrollX);
	}
}

restoreScrollTop = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		var t = sessionStorage.getItem(itemPrefix() + 'div.scroller.scrollTop');
		if(t)
		{
			el.scrollTop = t;
		}
	}
	
	var own_tab = null;
	if(parent.gui && (own_tab = parent.gui.find_tab_by_name(window.name)))
	{
		own_tab.loaded = true;
		own_tab.restore_scroll_position();
	}
	else
	{
		var y = sessionStorage.getItem(itemPrefix() + 'window.scrollY');
		var x = sessionStorage.getItem(itemPrefix() + 'window.scrollX');
		if(x && y)
		{
			window.scroll(x, y);
		}
	}
}

reloadPage = function()
{
	saveScrollTop();
	var own_tab = null;
	if(parent.gui && (own_tab = parent.gui.find_tab_by_name(window.name)))
	{
		// within tiled GUI
		own_tab.refresh(); // refresh my own tab
		parent.gui.refresh_active_tabs(); // refresh active tabs
	}
	else
	{
		// stand-alone
		window.location.replace(window.location.href); // reload without POST
	}
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

document.addEventListener('DOMContentLoaded', function(event) { resize(); restoreScrollTop(); setInterval(resize, window_resize_refresh_period); });
