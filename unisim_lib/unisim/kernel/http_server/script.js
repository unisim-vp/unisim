var min_width = 32;
var min_height = 32;
var m_pos_x;
var m_pos_y;
var content_div = document.getElementById("content-div");
var left_div = document.getElementById("left-div");
var left_horiz_resizer_div = document.getElementById("left-horiz-resizer-div");
var right_div = document.getElementById("right-div");
var top_right_iframe = document.getElementById("top-right-iframe");
var vert_resizer_div = document.getElementById("vert-resizer-div");
var bottom_div = document.getElementById("bottom-div");
var bottom_left_iframe = document.getElementById("bottom-left-iframe");
var bottom_horiz_resizer_div = document.getElementById("bottom-horiz-resizer-div");
var bottom_right_iframe = document.getElementById("bottom-right-iframe");

// left_div.setAttribute("scrolling", "auto");
// top_right_iframe.setAttribute("scrolling", "auto");
// bottom_left_iframe.setAttribute("scrolling", "auto");
// bottom_right_iframe.setAttribute("scrolling", "auto");
// 
// left_div.setAttribute("scrolling", "no");
// top_right_iframe.setAttribute("scrolling", "no");
// bottom_left_iframe.setAttribute("scrolling", "no");
// bottom_right_iframe.setAttribute("scrolling", "no");

function save_layout()
{
	sessionStorage.setItem('content-div.style.width', content_div.style.width);
	sessionStorage.setItem('left-div.style.width', left_div.style.width);
	sessionStorage.setItem('right-div.style.width', right_div.style.width);
	sessionStorage.setItem('top-right-iframe.style.width', top_right_iframe.style.width);
	sessionStorage.setItem('vert-resizer-div.style.width', vert_resizer_div.style.width);
	sessionStorage.setItem('bottom-div.style.width', bottom_div.style.width);
	sessionStorage.setItem('bottom-left-iframe.style.width', bottom_left_iframe.style.width);
	sessionStorage.setItem('bottom-right-iframe.style.width', bottom_right_iframe.style.width);

	sessionStorage.setItem('content-div.style.height', content_div.style.height);
	sessionStorage.setItem('left-div.style.height', left_div.style.height);
	sessionStorage.setItem('left-horiz-resizer-div.style.height', left_horiz_resizer_div.style.height);
	sessionStorage.setItem('right-div.style.height', right_div.style.height);
	sessionStorage.setItem('top-right-iframe.style.height', top_right_iframe.style.height);
	sessionStorage.setItem('bottom-div.style.height', bottom_div.style.height);
	sessionStorage.setItem('bottom-left-iframe.style.height', bottom_left_iframe.style.height);
	sessionStorage.setItem('bottom-horiz-resizer-div.style.height', bottom_horiz_resizer_div.style.height);
	sessionStorage.setItem('bottom-right-iframe.style.height', bottom_right_iframe.style.height);
}

function restore_layout()
{
	var content_div_width = sessionStorage.getItem('content-div.style.width');
	var left_div_width = sessionStorage.getItem('left-div.style.width');
	var right_div_width = sessionStorage.getItem('right-div.style.width');
	var top_right_iframe_width = sessionStorage.getItem('top-right-iframe.style.width');
	var vert_resizer_div_width = sessionStorage.getItem('vert-resizer-div.style.width');
	var bottom_div_width = sessionStorage.getItem('bottom-div.style.width');
	var bottom_left_iframe_width = sessionStorage.getItem('bottom-left-iframe.style.width');
	var bottom_right_iframe_width = sessionStorage.getItem('bottom-right-iframe.style.width');

	var content_div_height = sessionStorage.getItem('content-div.style.height');
	var left_div_height = sessionStorage.getItem('left-div.style.height');
	var left_horiz_resizer_div_height = sessionStorage.getItem('left-horiz-resizer-div.style.height');
	var right_div_height = sessionStorage.getItem('right-div.style.height');
	var top_right_iframe_height = sessionStorage.getItem('top-right-iframe.style.height');
	var bottom_div_height = sessionStorage.getItem('bottom-div.style.height');
	var bottom_left_iframe_height = sessionStorage.getItem('bottom-left-iframe.style.height');
	var bottom_horiz_resizer_div_height = sessionStorage.getItem('bottom-horiz-resizer-div.style.height');
	var bottom_right_iframe_height = sessionStorage.getItem('bottom-right-iframe.style.height');

	if(content_div_width !== null) content_div.style.width = content_div_width;
	if(left_div_width !== null) left_div.style.width = left_div_width;
	if(right_div_width !== null) right_div.style.width = right_div_width;
	if(top_right_iframe_width !== null) top_right_iframe.style.width = top_right_iframe_width;
	if(vert_resizer_div_width !== null) vert_resizer_div.style.width = vert_resizer_div_width;
	if(bottom_div_width !== null) bottom_div.style.width = bottom_div_width;
	if(bottom_left_iframe_width !== null) bottom_left_iframe.style.width = bottom_left_iframe_width;
	if(bottom_right_iframe_width !== null) bottom_right_iframe.style.width = bottom_right_iframe_width;

	if(content_div_height !== null) content_div.style.height = content_div_height;
	if(left_div_height !== null) left_div.style.height = left_div_height;
	if(left_horiz_resizer_div_height !== null) left_horiz_resizer_div.style.height = left_horiz_resizer_div_height;
	if(right_div_height !== null) right_div.style.height = right_div_height;
	if(top_right_iframe_height !== null) top_right_iframe.style.height = top_right_iframe_height;
	if(bottom_div_height !== null) bottom_div.style.height = bottom_div_height;
	if(bottom_left_iframe_height !== null) bottom_left_iframe.style.height = bottom_left_iframe_height;
	if(bottom_horiz_resizer_div_height !== null) bottom_horiz_resizer_div.style.height = bottom_horiz_resizer_div_height;
	if(bottom_right_iframe_height !== null) bottom_right_iframe.style.height = bottom_right_iframe_height;
}

function save_left_div_scroll_pos()
{
	sessionStorage.setItem('left-div.scrollTop', left_div.scrollTop);
}

function restore_left_div_scroll_pos()
{
	var left_div_scroll_top = sessionStorage.getItem('left-div.scrollTop');
	if(left_div_scroll_top !== null)
	{
		left_div.scrollTop = left_div_scroll_top;
	}
}

function resize_content(content_width, content_height)
{
	var left_div_width = parseInt(getComputedStyle(left_div, '').width);
	var left_horiz_resizer_div_width = parseInt(getComputedStyle(left_horiz_resizer_div, '').width);
	var right_div_width = parseInt(getComputedStyle(right_div, '').width);
	var vert_resizer_div_height = parseInt(getComputedStyle(vert_resizer_div, '').height);
	var bottom_div_height = parseInt(getComputedStyle(bottom_div, '').height);
	var bottom_left_iframe_width = parseInt(getComputedStyle(bottom_left_iframe, '').width);
	var bottom_horiz_resizer_div_width = parseInt(getComputedStyle(bottom_horiz_resizer_div, '').width);
	var bottom_right_iframe_width = parseInt(getComputedStyle(bottom_right_iframe, '').width);
	var top_right_iframe_height = parseInt(getComputedStyle(top_right_iframe, '').height);
	
	var min_content_width = min_width + left_horiz_resizer_div_width + min_width + bottom_horiz_resizer_div_width + min_width;
	var min_content_height = min_height + vert_resizer_div_height + min_height;
	if(content_width < min_content_width)
	{
		content_width = min_content_width;
	}
	if(content_height < min_content_height)
	{
		content_height = min_content_height;
	}

	bottom_right_iframe_width = Math.max(min_width, content_width - left_div_width - left_horiz_resizer_div_width - bottom_left_iframe_width - bottom_horiz_resizer_div_width);
	
	bottom_left_iframe_width = Math.max(min_width, content_width - left_div_width - left_horiz_resizer_div_width - bottom_horiz_resizer_div_width - bottom_right_iframe_width);
	
	var right_div_width = bottom_left_iframe_width + bottom_horiz_resizer_div_width + bottom_right_iframe_width;
	var bottom_div_width = right_div_width;
	var vert_resizer_div_width = right_div_width;
	var top_right_iframe_width = right_div_width;
	
	left_div_width = Math.max(min_width, content_width - left_horiz_resizer_div_width - right_div_width);

	bottom_div_height = Math.max(min_height, content_height - top_right_iframe_height - vert_resizer_div_height);

	var bottom_left_iframe_height = bottom_div_height;
	var bottom_right_iframe_height = bottom_div_height;
	var bottom_horiz_resizer_div_height = bottom_div_height;
	
	top_right_iframe_height = Math.max(min_height, content_height - vert_resizer_div_height - bottom_div_height);
	
	content_div.style.width = content_width + 'px';
	content_div.style.height = content_height + 'px';
	left_div.style.width = left_div_width + 'px';
	left_div.style.height = content_height + 'px';
	left_horiz_resizer_div.style.height = content_height + 'px';
	right_div.style.width = right_div_width + 'px';
	right_div.style.height = content_height + 'px';
	top_right_iframe.style.width = top_right_iframe_width + 'px';
	top_right_iframe.style.height = top_right_iframe_height + 'px';
	vert_resizer_div.style.width = vert_resizer_div_width + 'px';
	bottom_div.style.width = bottom_div_width + 'px';
	bottom_div.style.height = bottom_div_height + 'px';
	bottom_horiz_resizer_div.style.height = bottom_horiz_resizer_div_height + 'px';
	bottom_left_iframe.style.width = bottom_left_iframe_width + 'px';
	bottom_left_iframe.style.height = bottom_left_iframe_height + 'px';
	bottom_right_iframe.style.width = bottom_right_iframe_width + 'px';
	bottom_right_iframe.style.height = bottom_right_iframe_height + 'px';
	
	save_layout();
}

function left_horiz_resize(e)
{
	var dx = m_pos_x - e.x;
	m_pos_x = e.x;
	var left_div_width = parseInt(getComputedStyle(left_div, '').width);
	var right_div_width = parseInt(getComputedStyle(right_div, '').width);
	var top_right_iframe_width = parseInt(getComputedStyle(top_right_iframe, '').width);
	var vert_resizer_div_width = parseInt(getComputedStyle(vert_resizer_div, '').width);
	var bottom_div_width = parseInt(getComputedStyle(bottom_div, '').width);
	var bottom_left_iframe_width = parseInt(getComputedStyle(bottom_left_iframe, '').width);
	var bottom_horiz_resizer_width = parseInt(getComputedStyle(bottom_horiz_resizer_div, '').width);
	var bottom_right_iframe_width = parseInt(getComputedStyle(bottom_right_iframe, '').width);

	left_div_width = left_div_width - dx;
	right_div_width = right_div_width + dx;
	bottom_div_width = bottom_div_width + dx;
	top_right_iframe_width = top_right_iframe_width + dx;
	vert_resizer_div_width = vert_resizer_div_width + dx;
	bottom_left_iframe_width = bottom_left_iframe_width + dx;

	if((left_div_width >= min_width) && (right_div_width >= min_width) && (top_right_iframe_width >= min_width) && (vert_resizer_div_width >= min_width) && (bottom_left_iframe_width >= min_width))
	{
		left_div.style.width = left_div_width + "px";
		right_div.style.width = right_div_width + "px";
		bottom_div.style.width = bottom_div_width + "px";
		top_right_iframe.style.width = top_right_iframe_width + "px";
		vert_resizer_div.style.width = vert_resizer_div_width + "px";
		bottom_left_iframe.style.width = bottom_left_iframe_width + "px";
		
		save_layout();
	}
}

function vert_resize(e)
{
	var dy = m_pos_y - e.y;
	m_pos_y = e.y;
	var top_right_iframe_height = parseInt(getComputedStyle(top_right_iframe, '').height);
	var bottom_div_height = parseInt(getComputedStyle(bottom_div, '').height);
	var bottom_left_iframe_height = parseInt(getComputedStyle(bottom_left_iframe, '').height);
	var bottom_horiz_resizer_div_height = parseInt(getComputedStyle(bottom_horiz_resizer_div, '').height);
	var bottom_right_iframe_height = parseInt(getComputedStyle(bottom_right_iframe, '').height);

	top_right_iframe_height = top_right_iframe_height - dy;
	bottom_div_height = bottom_div_height + dy;
	bottom_left_iframe_height = bottom_left_iframe_height + dy;
	bottom_horiz_resizer_div_height = bottom_horiz_resizer_div_height + dy;
	bottom_right_iframe_height = bottom_right_iframe_height + dy;

	if((top_right_iframe_height >= min_height) && (bottom_div_height >= min_height) && (bottom_left_iframe_height >= min_height) && (bottom_horiz_resizer_div_height >= min_height) && (bottom_right_iframe_height >= min_height))
	{
		top_right_iframe.style.height = top_right_iframe_height + "px";
		bottom_div.style.height = bottom_div_height + "px";
		bottom_left_iframe.style.height = bottom_left_iframe_height + "px";
		bottom_horiz_resizer_div.style.height = bottom_horiz_resizer_div_height + "px";
		bottom_right_iframe.style.height = bottom_right_iframe_height + "px";
		
		save_layout();
	}
}

function bottom_horiz_resize(e)
{
	var dx = m_pos_x - e.x;
	m_pos_x = e.x;
	var bottom_left_iframe_width = parseInt(getComputedStyle(bottom_left_iframe, '').width);
	var bottom_right_iframe_width = parseInt(getComputedStyle(bottom_right_iframe, '').width);

	bottom_left_iframe_width = bottom_left_iframe_width - dx;
	bottom_right_iframe_width = bottom_right_iframe_width + dx;

	if((bottom_left_iframe_width >= min_width) && (bottom_right_iframe_width >= min_width))
	{
		bottom_left_iframe.style.width = bottom_left_iframe_width + "px";
		bottom_right_iframe.style.width = bottom_right_iframe_width + "px";
		
		save_layout();
	}
}

restore_layout();
restore_left_div_scroll_pos();

left_horiz_resizer_div.addEventListener("mousedown", function(e) {
	m_pos_x = e.x;
	left_div.style.pointerEvents='none';
	right_div.style.pointerEvents='none';
	top_right_iframe.style.pointerEvents='none';
	vert_resizer_div.style.pointerEvents='none';
	bottom_left_iframe.style.pointerEvents='none';
	bottom_horiz_resizer_div.pointerEvents='none';
	bottom_right_iframe.style.pointerEvents='none';
	document.addEventListener("mousemove", left_horiz_resize);
});

vert_resizer_div.addEventListener("mousedown", function(e) {
	m_pos_y = e.y;
	left_div.style.pointerEvents='none';
	left_horiz_resizer_div.style.pointerEvents='none';
	top_right_iframe.style.pointerEvents='none';
	bottom_left_iframe.style.pointerEvents='none';
	bottom_horiz_resizer_div.style.pointerEvents='none';
	bottom_right_iframe.style.pointerEvents='none';
	document.addEventListener("mousemove", vert_resize);
});

bottom_horiz_resizer_div.addEventListener("mousedown", function(e) {
	m_pos_x = e.x;
	left_div.style.pointerEvents='none';
	left_horiz_resizer_div.style.pointerEvents='none';
	top_right_iframe.style.pointerEvents='none';
	vert_resizer_div.style.pointerEvents='none';
	bottom_left_iframe.style.pointerEvents='none';
	bottom_right_iframe.style.pointerEvents='none';
	document.addEventListener("mousemove", bottom_horiz_resize);
});

document.addEventListener("mouseup", function() {
	left_div.style.pointerEvents='auto';
	left_horiz_resizer_div.style.pointerEvents='auto';
	right_div.style.pointerEvents='auto';
	top_right_iframe.style.pointerEvents='auto';
	vert_resizer_div.style.pointerEvents='auto';
	bottom_left_iframe.style.pointerEvents='auto'; 
	bottom_horiz_resizer_div.style.pointerEvents='auto';
	bottom_right_iframe.style.pointerEvents='auto';
	document.removeEventListener("mousemove", left_horiz_resize);
	document.removeEventListener("mousemove", vert_resize);
	document.removeEventListener("mousemove", bottom_horiz_resize);
});

// left_div.addEventListener("mouseenter", function() {
// 	left_div.setAttribute("scrolling", "auto");
// });
// 
// left_div.addEventListener("mouseleave", function() {
// 	left_div.setAttribute("scrolling", "no");
// });

top_right_iframe.addEventListener("mouseenter", function() {
	top_right_iframe.setAttribute("scrolling", "auto");
});

top_right_iframe.addEventListener("mouseleave", function() {
	top_right_iframe.setAttribute("scrolling", "no");
});

bottom_left_iframe.addEventListener("mouseenter", function() {
	bottom_left_iframe.setAttribute("scrolling", "auto");
});

bottom_left_iframe.addEventListener("mouseleave", function() {
	bottom_left_iframe.setAttribute("scrolling", "no");
});

bottom_right_iframe.addEventListener("mouseenter", function() {
	bottom_right_iframe.setAttribute("scrolling", "auto");
});

bottom_right_iframe.addEventListener("mouseleave", function() {
	bottom_right_iframe.setAttribute("scrolling", "no");
});

var window_inner_width = window.innerWidth;
var window_inner_height = window.innerHeight;
resize_content(window_inner_width, window_inner_height);

setInterval(function () {
	var new_window_inner_width = window.innerWidth;
	var new_window_inner_height = window.innerHeight;
	if((new_window_inner_width != window_inner_width) || (new_window_inner_height != window_inner_height))
	{
		window_inner_width = new_window_inner_width;
		window_inner_height = new_window_inner_height;
		resize_content(window_inner_width, window_inner_height);
	}
	
}, 1000);
