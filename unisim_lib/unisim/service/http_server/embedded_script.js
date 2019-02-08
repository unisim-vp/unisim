find_own_tab = function()
{
	if(!parent.gui) return false;
	if(parent.gui.magic != 0xCAFE) return false;
	return parent.gui.find_tab_by_iframe_name(window.name);
}

storage_item_prefix = function()
{
	var own_tab = find_own_tab();
	return own_tab ? own_tab.name : '';
}

save_window_scroll_top = function()
{
	var own_tab = find_own_tab();
	if(own_tab)
	{
		// within tiled GUI
		own_tab.save_scroll_position();
	}
	else
	{
		sessionStorage.setItem(storage_item_prefix() + 'window.scrollY', window.scrollY);
		sessionStorage.setItem(storage_item_prefix() + 'window.scrollX', window.scrollX);
	}
}

restore_window_scroll_top = function()
{
	var own_tab = find_own_tab();
	if(own_tab)
	{
		// within tiled GUI
		own_tab.loaded = true;
		own_tab.restore_scroll_position();
	}
	else
	{
		// stand-alone
		var y = sessionStorage.getItem(storage_item_prefix() + 'window.scrollY');
		var x = sessionStorage.getItem(storage_item_prefix() + 'window.scrollX');
		if(x && y)
		{
			window.scroll(x, y);
		}
	}
}

reload_page = function()
{
	save_window_scroll_top();
	var own_tab = find_own_tab();
	if(own_tab)
	{
		// within tiled GUI
		own_tab.refresh(); // refresh my own tab
	}
	else
	{
		// stand-alone
		window.location.replace(window.location.href); // reload without POST
	}
}

reload_third_tabs = function()
{
	var own_tab = find_own_tab();
	if(own_tab)
	{
		// within tiled GUI
		parent.gui.for_each_tab(
			function(tab)
			{
				if(tab.is_active && (tab != own_tab)) tab.refresh(); // refresh other tabs that are active
			}
		);
		
		parent.gui.for_each_child_window(
			function(child_window)
			{
				child_window.location.replace(child_window.location.href); // reload without POST
			}
		);
	}
}

get_next_target = function()
{
	var own_tab = find_own_tab();
	return own_tab ? own_tab.get_next_target(window.name) : '_self';
}
