saveScrollTop = function()
{
	var el = document.querySelector('div.scroller');
	if(el)
	{
		sessionStorage.setItem('div.scroller.scrollTop', el.scrollTop);
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
}
