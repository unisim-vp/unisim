saveScrollTop = function()
{
	var el = document.querySelector("div.instruments-table-container");
	if(el)
	{
		sessionStorage.setItem('div.instruments-table-container.scrollTop', el.scrollTop);
	}
}

restoreScrollTop = function()
{
	var el = document.querySelector("div.instruments-table-container");
	if(el)
	{
		var t = sessionStorage.getItem('div.instruments-table-container.scrollTop');
		if(t)
		{
			el.scrollTop = t;
		}
	}
}
