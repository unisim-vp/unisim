item_prefix = function()
{
	return (window.frameElement && window.name) ? (window.name + '.') : '';
}

save_window_scroll_top = function()
{
	var own_tab = null;
	if(parent.gui && (parent.gui.magic == 0xCAFE) && (own_tab = parent.gui.find_tab_by_name(window.name)))
	{
		// within tiled GUI
		own_tab.save_scroll_position();
	}
	else
	{
		sessionStorage.setItem(item_prefix() + 'window.scrollY', window.scrollY);
		sessionStorage.setItem(item_prefix() + 'window.scrollX', window.scrollX);
	}
}

restore_window_scroll_top = function()
{
	var own_tab = null;
	if(parent.gui && (parent.gui.magic == 0xCAFE) && (own_tab = parent.gui.find_tab_by_name(window.name)))
	{
		// within tiled GUI
		own_tab.loaded = true;
		own_tab.restore_scroll_position();
	}
	else
	{
		// stand-alone
		var y = sessionStorage.getItem(item_prefix() + 'window.scrollY');
		var x = sessionStorage.getItem(item_prefix() + 'window.scrollX');
		if(x && y)
		{
			window.scroll(x, y);
		}
	}
}

reload_page = function()
{
	save_window_scroll_top();
	var own_tab = null;
	if(parent.gui && (parent.gui.magic == 0xCAFE) && (own_tab = parent.gui.find_tab_by_name(window.name)))
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