
saveScrollTop = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		sessionStorage.setItem('div.scroller.scrollTop', el.scrollTop);
	}
	
	var own_tab = null;
	if(parent.gui && (own_tab = parent.gui.find_tab_by_name(window.name)))
	{
		// within tiled GUI
		own_tab.save_scroll_position();
	}
	else
	{
		sessionStorage.setItem('window.scrollY', window.scrollY);
		sessionStorage.setItem('window.scrollX', window.scrollX);
	}
}

restoreScrollTop = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		var t = sessionStorage.getItem('div.scroller.scrollTop');
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
		var y = sessionStorage.getItem('window.scrollY');
		var x = sessionStorage.getItem('window.scrollX');
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
