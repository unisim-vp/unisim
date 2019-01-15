// https://tc39.github.io/ecma262/#sec-array.prototype.find
if (!Array.prototype.find) {
  Object.defineProperty(Array.prototype, 'find', {
    value: function(predicate) {
     // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If IsCallable(predicate) is false, throw a TypeError exception.
      if (typeof predicate !== 'function') {
        throw new TypeError('predicate must be a function');
      }

      // 4. If thisArg was supplied, let T be thisArg; else let T be undefined.
      var thisArg = arguments[1];

      // 5. Let k be 0.
      var k = 0;

      // 6. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kValue be ? Get(O, Pk).
        // c. Let testResult be ToBoolean(? Call(predicate, T, « kValue, k, O »)).
        // d. If testResult is true, return kValue.
        var kValue = o[k];
        if (predicate.call(thisArg, kValue, k, o)) {
          return kValue;
        }
        // e. Increase k by 1.
        k++;
      }

      // 7. Return undefined.
      return undefined;
    },
    configurable: true,
    writable: true
  });
}

// https://tc39.github.io/ecma262/#sec-array.prototype.findindex
if (!Array.prototype.findIndex) {
  Object.defineProperty(Array.prototype, 'findIndex', {
    value: function(predicate) {
     // 1. Let O be ? ToObject(this value).
      if (this == null) {
        throw new TypeError('"this" is null or not defined');
      }

      var o = Object(this);

      // 2. Let len be ? ToLength(? Get(O, "length")).
      var len = o.length >>> 0;

      // 3. If IsCallable(predicate) is false, throw a TypeError exception.
      if (typeof predicate !== 'function') {
        throw new TypeError('predicate must be a function');
      }

      // 4. If thisArg was supplied, let T be thisArg; else let T be undefined.
      var thisArg = arguments[1];

      // 5. Let k be 0.
      var k = 0;

      // 6. Repeat, while k < len
      while (k < len) {
        // a. Let Pk be ! ToString(k).
        // b. Let kValue be ? Get(O, Pk).
        // c. Let testResult be ToBoolean(? Call(predicate, T, « kValue, k, O »)).
        // d. If testResult is true, return k.
        var kValue = o[k];
        if (predicate.call(thisArg, kValue, k, o)) {
          return k;
        }
        // e. Increase k by 1.
        k++;
      }

      // 7. Return -1.
      return -1;
    },
    configurable: true,
    writable: true
  });
}

var isIE = !!navigator.userAgent.match(/Trident/g) || !!navigator.userAgent.match(/MSIE/g);

getWidth = function(el)
{
	var style = getComputedStyle(el, null);
	var width = Math.ceil(parseFloat(style.getPropertyValue('width')));
	if(isIE)
	{
		var border_left_width = parseFloat(style.getPropertyValue('border-left-width'));
		var border_right_width = parseFloat(style.getPropertyValue('border-right-width'));
		
		return width + border_left_width + border_right_width;
	}
	
	return width;
}

getHeight = function(el)
{
	var style = getComputedStyle(el, null);
	var height = Math.ceil(parseFloat(style.getPropertyValue('height')));
	if(isIE)
	{
		var border_top_width = parseFloat(style.getPropertyValue('border-top-width'));
		var border_bottom_width = parseFloat(style.getPropertyValue('border-bottom-width'));
		
		return height + border_top_width + border_bottom_width;
	}
	
	return height;
}

// Point
Point.prototype.x = null;
Point.prototype.y = null;

function Point(x, y)
{
	this.x = x || 0;
	this.y = y || 0;
}

// TabConfig
TabConfig.prototype.label = null;
TabConfig.prototype.name = null;
TabConfig.prototype.src = null;
TabConfig.prototype.object_name = null;
TabConfig.prototype.query = null;

function TabConfig(label, name, src, object_name)
{
	this.label = label;
	this.name = name;
	this.src = src;
	this.object_name = object_name;
	this.query = null;
}

TabConfig.prototype.save = function(prefix)
{
	if(this.label) sessionStorage.setItem(prefix + '.label', this.label);
	if(this.name) sessionStorage.setItem(prefix + '.name', this.name);
	if(this.src) sessionStorage.setItem(prefix + '.src', this.src);
	if(this.object_name) sessionStorage.setItem(prefix + '.object-name', this.object_name);
	if(this.query) sessionStorage.setItem(prefix + '.query', this.query);
}

TabConfig.prototype.restore = function(prefix)
{
	this.label = sessionStorage.getItem(prefix + '.label');
	this.name = sessionStorage.getItem(prefix + '.name');
	this.src = sessionStorage.getItem(prefix + '.src');
	this.object_name = sessionStorage.getItem(prefix + '.object-name');
	this.query = sessionStorage.getItem(prefix + '.query');
}

// TabConfigs
TabConfigs.prototype.tab_configs = null;

function TabConfigs()
{
	this.tab_configs = new Array();
}

TabConfigs.prototype.push = function(tab_config)
{
	if(!this.find_tab_config(tab_config))
	{
		this.tab_configs.push(tab_config);
	}
}

TabConfigs.prototype.remove = function(tab_config)
{
	var tab_config_index = this.tab_configs.findIndex(function(other_tab_config) { return other_tab_config == tab_config; });
	if(tab_config_index >= 0)
	{
		this.tab_configs.splice(tab_config_index, 1);
	}
}

TabConfigs.prototype.save = function(prefix)
{
	sessionStorage.setItem(prefix + '.length', this.tab_configs.length);
	this.tab_configs.forEach(function(tab_config, index) { tab_config.save(prefix + '.tab' + index); });
}

TabConfigs.prototype.restore = function(prefix)
{
	var length = sessionStorage.getItem(prefix + '.length') || 0;
	for(var i = 0; i < length; i++)
	{
		var tab_config = new TabConfig();
		tab_config.restore(prefix + '.tab' + i);
		if(!this.find_tab_config_by_name(tab_config.name))
		{
			this.tab_configs.push(tab_config);
		}
	}
}

TabConfigs.prototype.find_tab_config = function(tab_config)
{
	return this.tab_configs.find(function(other_tab_config) { return other_tab_config == tab_config; });
}

TabConfigs.prototype.find_tab_config_by_name = function(name)
{
	return this.tab_configs.find(function(tab_config) { return tab_config.name == name; });
}

// Tab
Tab.prototype.tab_config = null;
Tab.prototype.static = false;
Tab.prototype.tab_header = null;
Tab.prototype.tab_content = null;
Tab.prototype.tab_content_bg = null;
Tab.prototype.close_tab = null;
Tab.prototype.bound_close = null;
Tab.prototype.is_active = false;
Tab.prototype.loaded = false;
Tab.prototype.bound_tab_header_onclick = null;

function Tab(owner)
{
	this.owner = owner;
}

Tab.prototype.create = function(tab_config)
{
	this.static = false;
	this.tab_config = tab_config;
	this.close_tab = document.createElement('div');
	this.close_tab.className = 'close-tab';
	this.tab_header = document.createElement('div');
	this.tab_header.className = 'tab-header noselect';
	this.tab_header.textContent = this.tab_config.label;
	this.tab_header.appendChild(this.close_tab);
	this.owner.tab_headers.appendChild(this.tab_header);
	this.tab_content = document.createElement('iframe');
	this.tab_content.className = 'tab-content';
	this.tab_content.setAttribute('name', this.tab_config.name);
	this.tab_content.setAttribute('id', this.tab_config.name);
	this.loaded = false;
	this.tab_content.addEventListener('load', this.tab_content_on_load.bind(this));
	this.restore_scroll_position();
	if(this.tab_config.src)
	{
		this.tab_content.setAttribute('src', this.tab_config.src);
	}
	this.owner.tab_contents.appendChild(this.tab_content);
	
	this.bound_tab_header_onclick = this.tab_header_onclick.bind(this);
	this.tab_header.addEventListener('click', this.bound_tab_header_onclick, false);
	
	this.bound_close = this.close.bind(this);
	this.close_tab.addEventListener('click', this.bound_close);

	this.tab_content.addEventListener('mouseenter', this.tab_content_on_mouseenter.bind(this));
	this.tab_content.addEventListener('mouseleave', this.tab_content_on_mouseleave.bind(this));
}

Tab.prototype.reuse_static = function(tab_header, tab_content)
{
	this.static = true;
	this.tab_config = new TabConfig();
	this.tab_header = tab_header;
	this.tab_content = tab_content;
	if(this.tab_content.hasAttribute('name'))
	{
		var name = this.tab_content.getAttribute('name');
		this.tab_config.name = name;
	}
	else if(this.tab_content.hasAttribute('id'));
	{
		var id = this.tab_content.getAttribute('id');
		this.tab_config.name = id;
	}
	this.bound_tab_header_onclick = this.tab_header_onclick.bind(this);
	this.tab_header.addEventListener('click', this.bound_tab_header_onclick, false);

	this.bound_close = this.close.bind(this);
	var close_tabs = this.tab_header.getElementsByClassName('close-tab');
	if(close_tabs.length != 0)
	{
		this.close_tab = close_tabs[0];
		this.close_tab.addEventListener('click', this.bound_close);
	}
}

Tab.prototype.destroy = function()
{
	if(!this.static)
	{
		this.owner.tab_headers.removeChild(this.tab_header);
		this.owner.tab_contents.removeChild(this.tab_content);
	}
	this.tab_header.removeEventListener('click', this.bound_tab_header_onclick);
	if(this.close_tab)
	{
		this.close_tab.removeEventListener('click', this.bound_close);
	}
	this.owner = null;
	this.tab_config = null;
	this.tab_header = null;
	this.tab_content = null;
	this.close_tab = null;
	this.bound_close = null;
	this.bound_tab_header_onclick = null;
}

Tab.prototype.save = function(prefix)
{
	this.tab_config.save(prefix);
}

Tab.prototype.restore = function(prefix)
{
	this.tab_config.restore(prefix);
}

Tab.prototype.update_src = function()
{
	if(this.tab_config.src)
	{
		if(this.tab_content.hasAttribute('src'))
		{
			this.loaded = false;
			var src_attr = this.tab_config.src;
			if(this.tab_config.query) src_attr += '?' + this.tab_config.query;
			this.tab_content.setAttribute('src', src_attr);
		}
	}
}

Tab.prototype.refresh = function()
{
	this.save_scroll_position();
	this.update_src();
}

Tab.prototype.tab_content_on_load = function(event)
{
	this.loaded = true;
	this.restore_scroll_position();
}

Tab.prototype.bind_to_object = function(object_name)
{
	this.save_scroll_position();
	this.tab_config.object_name = object_name;
	if(object_name)
	{
		this.tab_config.query = 'object=' + encodeURIComponent(object_name);
	}
	else
	{
		this.tab_config.query = null;
	}
	this.update_src();
}

Tab.prototype.save_scroll_position = function()
{
	if(!this.loaded) return;
	
	if(!this.tab_content.contentWindow) return;
	
	if(!this.tab_content.contentDocument) return;
	
	if(!this.static)
	{
		var scroll_x_item_name = 'gui.' + this.owner.name + '.' + this.tab_config.name;
		if(this.tab_config.object_name && (this.tab_config.name != this.tab_config.object_name)) scroll_x_item_name += '.' + this.tab_config.object_name;
		scroll_x_item_name += '.scroll-x';
		
		var scroll_y_item_name = 'gui.' + this.owner.name + '.' + this.tab_config.name;
		if(this.tab_config.object_name && (this.tab_config.name != this.tab_config.object_name)) scroll_y_item_name += '.' + this.tab_config.object_name;
		scroll_y_item_name += '.scroll-y';

		var scroll_x = isIE ? this.tab_content.contentDocument.documentElement.scrollLeft : this.tab_content.contentWindow.scrollX;
		var scroll_y = isIE ? this.tab_content.contentDocument.documentElement.scrollTop : this.tab_content.contentWindow.scrollY;
		sessionStorage.setItem(scroll_x_item_name, scroll_x);
		sessionStorage.setItem(scroll_y_item_name, scroll_y);
	}
}

Tab.prototype.restore_scroll_position = function()
{
	if(!this.loaded) return;
	
	if(!this.tab_content.contentWindow) return;
	
	if(!this.tab_content.contentDocument) return;

	if(!this.static)
	{
		var scroll_x_item_name = 'gui.' + this.owner.name + '.' + this.tab_config.name;
		if(this.tab_config.object_name && (this.tab_config.name != this.tab_config.object_name)) scroll_x_item_name += '.' + this.tab_config.object_name;
		scroll_x_item_name += '.scroll-x';
		
		var scroll_y_item_name = 'gui.' + this.owner.name + '.' + this.tab_config.name;
		if(this.tab_config.object_name && (this.tab_config.name != this.tab_config.object_name)) scroll_y_item_name += '.' + this.tab_config.object_name;
		scroll_y_item_name += '.scroll-y';
		
		var scroll_x = sessionStorage.getItem(scroll_x_item_name);
		var scroll_y = sessionStorage.getItem(scroll_y_item_name);
		
		if(scroll_x && scroll_y)
		{
			if(isIE)
			{
				this.tab_content.contentDocument.documentElement.scrollLeft = scroll_x;
				this.tab_content.contentDocument.documentElement.scrollTop = scroll_y;
			}
			else
			{
				this.tab_content.contentWindow.scrollTo(scroll_x, scroll_y);
			}
		}
	}
}

Tab.prototype.set_active = function(v)
{
	if(this.is_active != v)
	{
		if(v)
		{
			this.tab_header.classList.add('tab-header-active');
			this.tab_content.style.visibility = 'visible';
			this.owner.owner.on_tab_header_switch(this);
		}
		else
		{
			this.tab_header.classList.remove('tab-header-active');
			this.tab_content.style.visibility = 'hidden';
		}
		this.is_active = v;
	}
}

Tab.prototype.enable_pointer_events = function(v)
{
	if(!this.static)
	{
		this.tab_content.style.pointerEvents = (((v === undefined) || v) ? 'auto' : 'none');
	}
}

Tab.prototype.switch_to = function()
{
	this.owner.switch_to(this);
	this.refresh();
}

Tab.prototype.tab_header_onclick = function(event)
{
	this.switch_to();
}

Tab.prototype.close = function(event)
{
	this.owner.destroy_tab(this);
	event.stopImmediatePropagation(); // avoid calling of this.tab_header_onclick after tab destruction
}

Tab.prototype.tab_content_on_mouseenter = function(event)
{
	this.tab_content.setAttribute('scrolling', 'yes');
	this.tab_content.style.overflow = 'auto';
}

Tab.prototype.tab_content_on_mouseleave = function(event)
{
	this.tab_content.setAttribute('scrolling', 'no');
	this.tab_content.style.overflow = 'hidden';
}

// Tabs
Tabs.prototype.owner = null;
Tabs.prototype.name = null;
Tabs.prototype.tile = null;
Tabs.prototype.tab_headers = null;
Tabs.prototype.tab_contents = null;
Tabs.prototype.tab_headers_left_scroller = null;
Tabs.prototype.tab_headers_right_scroller = null;
Tabs.prototype.tabs = null;
Tabs.prototype.tile_width = 0;
Tabs.prototype.controls_width = 0;
Tabs.prototype.tab_headers_width = 0;
Tabs.prototype.tab_config_history = null;
Tabs.prototype.history_shortcut_element = null;
Tabs.prototype.history_element = null;

function Tabs(owner, name, tile)
{
	this.owner = owner;
	this.name = name;
	this.tile = tile;
	this.tab_headers = this.tile.getElementsByClassName('tab-headers')[0];
	this.tab_contents = this.tile.getElementsByClassName('tab-contents')[0];
	this.history_shortcut_element = this.tile.getElementsByClassName('tab-headers-history-shortcut')[0];
	this.tab_headers_left_scroller = this.tile.getElementsByClassName('tab-headers-left-scroller')[0];
	this.tab_headers_right_scroller = this.tile.getElementsByClassName('tab-headers-right-scroller')[0];
	this.tabs = new Array();
	this.tab_config_history = new TabConfigs();
	this.history_element = null;
	var saved_tab_configs = new TabConfigs();
	saved_tab_configs.restore('gui.' + this.name);
	
	var history_shortcut_element_width = getWidth(this.history_shortcut_element);
	var left_scroller_width = getWidth(this.tab_headers_left_scroller);
	var right_scroller_width = getWidth(this.tab_headers_right_scroller);
	this.controls_width = history_shortcut_element_width + left_scroller_width + right_scroller_width;
	
	// reuse static tabs
	var static_tab_headers = this.tab_headers.getElementsByClassName('tab-header');
	var static_tab_contents = this.tab_contents.getElementsByClassName('tab-content');
	
	for(var i = 0; (i < static_tab_headers.length) && (i < static_tab_contents.length); i++)
	{
		var tab = new Tab(this);
		var tab_header = static_tab_headers[i];
		var tab_content = static_tab_contents[i];
		tab.reuse_static(tab_header, tab_content);
		var saved_tab_config = saved_tab_configs.find_tab_config_by_name(tab.tab_config.name);
		if(saved_tab_config)
		{
			tab.tab_config = saved_tab_config;
			saved_tab_configs.remove(saved_tab_config);
		}
		if(!this.find_tab_by_name(tab.tab_config.name))
		{
			this.tabs.push(tab);
			tab.tab_header.addEventListener('click', this.switch_to.bind(this, tab), false);
			this.save_tabs();
		}
	}
	
	// recreate tabs from saved tab configs
	saved_tab_configs.tab_configs.forEach(function(saved_tab_config) { this.create_tab_from_config(saved_tab_config); }, this);
	
	this.compute_dynamic_widths();
	if(this.tab_headers_left_scroller) this.tab_headers_left_scroller.addEventListener('click', this.scroll_tab_headers.bind(this, -32));
	if(this.tab_headers_right_scroller) this.tab_headers_right_scroller.addEventListener('click', this.scroll_tab_headers.bind(this, +32));
	
	if(this.history_shortcut_element) this.history_shortcut_element.addEventListener('click', this.history_shortcut_onclick.bind(this));
	
	this.restore_history();
	this.restore_active_tab();
	this.restore_tab_headers_left();
}

Tabs.prototype.save_active_tab = function(tab)
{
	sessionStorage.setItem('gui.' + this.name + '.active-tab', tab.tab_config.name);
}

Tabs.prototype.save_tabs = function()
{
	sessionStorage.setItem('gui.' + this.name + '.length', this.tabs.length);
	for(var i = 0; i < this.tabs.length; i++)
	{
		var tab = this.tabs[i];
		tab.save('gui.' + this.name + '.tab' + i);
	}
}

Tabs.prototype.save_history = function()
{
	this.tab_config_history.save('gui.' + this.name + '.history');
}

Tabs.prototype.save_tab_headers_left = function()
{
	sessionStorage.setItem('gui.' + this.name + '.tab-headers.style.left', this.tab_headers.style.left);
}

Tabs.prototype.restore_active_tab = function()
{
	var name = sessionStorage.getItem('gui.' + this.name + '.active-tab');
	var tab = this.find_tab_by_name(name);
	if(tab)
	{
		tab.switch_to();
	}
}

Tabs.prototype.restore_history = function()
{
	this.tab_config_history.restore('gui.' + this.name + '.history');
}

Tabs.prototype.restore_tab_headers_left = function()
{
	var tab_headers_left = sessionStorage.getItem('gui.' + this.name + '.tab-headers.style.left');
	if(tab_headers_left) this.tab_headers.style.left = tab_headers_left;
}

Tabs.prototype.compute_dynamic_widths = function()
{
	this.tile_width = this.tile_width = getWidth(this.tile);
	this.tab_headers_width = 0;
	var tab_headers = this.tab_headers.getElementsByClassName('tab-header');
	for(var i = 0; i < tab_headers.length; i++)
	{
		var tab_header = tab_headers[i];
		var tab_header_width = getWidth(tab_header);
		this.tab_headers_width = this.tab_headers_width + tab_header_width;
	}
}

Tabs.prototype.create_tab = function(label, name, src, object_name)
{
	var tab = this.find_tab_by_name(name);
	if(!tab)
	{
		tab = new Tab(this);
		var tab_config = this.tab_config_history.find_tab_config_by_name(name);
		if(tab_config)
		{
			tab_config.label = label;
			tab_config.src = src;
			tab_config.object_name = object_name;
			this.tab_config_history.remove(tab_config);
			this.save_history();
		}
		else
		{
			tab_config = new TabConfig(label, name, src, object_name)
		}
		tab.create(tab_config);
		this.tabs.push(tab);
		tab.tab_header.addEventListener('click', this.switch_to.bind(this, tab), false);
		this.save_tabs();
	}
	return tab;
}

Tabs.prototype.create_tab_from_config = function(tab_config)
{
	var tab = this.find_tab_by_name(tab_config.name);
	
	if(!tab)
	{
		tab = new Tab(this);
		tab.create(tab_config);
		this.tabs.push(tab);
		this.save_tabs();
	}
	return tab;
}

Tabs.prototype.destroy_tab = function(tab_to_destroy)
{
	var tab_num = this.tabs.findIndex(function(tab) { return tab == tab_to_destroy; });
	
	if(tab_num >= 0)
	{
		this.tab_config_history.push(tab_to_destroy.tab_config);
		var is_active = tab_to_destroy.is_active;
		this.tabs.splice(tab_num, 1);
		tab_to_destroy.destroy();
		this.save_tabs();
		this.save_history();
		if(is_active && (this.tabs.length != 0))
		{
			this.switch_to_nth_tab((tab_num > 0) ? (tab_num - 1) : tab_num);
		}
	}
}

Tabs.prototype.destroy_nth_tab = function(tab_num)
{
	this.destroy_tab(this.tabs[tab_num]);
}

Tabs.prototype.switch_to = function(tab)
{
	for(var tab_num = 0; tab_num < this.tabs.length; tab_num++)
	{
		var is_active = (this.tabs[tab_num] == tab);
		this.tabs[tab_num].set_active(is_active);
		if(is_active)
		{
			this.save_active_tab(tab);
		}
	}
}

Tabs.prototype.switch_to_nth_tab = function(tab_num)
{
	if(tab_num < this.tabs.length)
	{
		this.switch_to(this.tabs[tab_num]);
	}
}

Tabs.prototype.enable_pointer_events = function(v)
{
	this.tabs.forEach(function(tab) { tab.enable_pointer_events(v); });
}

Tabs.prototype.find_tab = function(tab)
{
	return this.tabs.find(function(elt) { return elt == tab; });
}

Tabs.prototype.find_tab_by_name = function(name)
{
	return this.tabs.find(function(tab) { return tab.tab_config.name == name; });
}

Tabs.prototype.find_tab_by_object_name = function(object_name)
{
	return this.tabs.find(function(tab) { return tab.tab_config.object_name == object_name; });
}

Tabs.prototype.bind_to_object = function(object_name)
{
	this.tabs.forEach(function(tab) { tab.bind_to_object(object_name); });
	this.save_tabs();
}

Tabs.prototype.scroll_tab_headers = function(dx)
{
	this.compute_dynamic_widths();
	var left = parseFloat(getComputedStyle(this.tab_headers, '').left);
	left = left + dx;
	left = Math.max(left, this.tile_width - this.tab_headers_width);
	left = Math.min(this.controls_width, left);
	this.tab_headers.style.left = left + 'px';
	this.save_tab_headers_left();
}

Tabs.prototype.history_shortcut_onclick = function()
{
	if(this.history_element)
	{
		this.tile.removeChild(this.history_element);
		this.history_element = null;
	}
	else if(this.tab_config_history.tab_configs.length > 0)
	{
		// create an 'history' below the 'history shortcut'
		this.history_element = document.createElement('div');
		this.history_element.className = 'tab-headers-history';
		this.history_element.style.position = 'absolute';
		this.history_element.style.left = this.history_shortcut_element.offsetLeft + 'px';
		this.history_element.style.top = (this.history_shortcut_element.offsetTop + this.history_shortcut_element.offsetHeight) + 'px';
		
		this.history_element.className = 'tab-headers-history';
		for(var i = 0; i < this.tab_config_history.tab_configs.length; i++)
		{
			var tab_config = this.tab_config_history.tab_configs[i];
			var history_item_element = document.createElement('div');
			history_item_element.className = 'tab-headers-history-item';
			history_item_element.textContent = tab_config.label;
			history_item_element.addEventListener('click', this.history_item_element_on_click.bind(this, tab_config));
			this.history_element.appendChild(history_item_element);
		}
		this.tile.appendChild(this.history_element);
	}
}

Tabs.prototype.history_item_element_on_click = function(tab_config)
{
	if(this.history_element)
	{
		this.tile.removeChild(this.history_element);
		this.history_element = null;
	}
	this.create_tab_from_config(tab_config).switch_to();
	this.tab_config_history.remove(tab_config);
	this.save_history();
}

Tabs.prototype.refresh_tab_by_name = function(name)
{
	var tab = this.find_tab_by_name(name);
	if(tab)
	{
		tab.refresh();
	}
}

Tabs.prototype.refresh_active_tabs = function()
{
	var active_tab = this.tabs.find(function(tab) { return tab.is_active; });
	if(active_tab) active_tab.refresh();
}

// GUI
// +---------------------------------------------------------------------------------------------------------------------------------------+
// | content-div                                                                                                                           |
// |  +-------------------------------+  +-+  +------------------------------------------------------------------------------------------+ |
// |  | left-tile-div                 |  | |  | right-div                                                                                | |
// |  | +---------------------------+ |  | |  | +--------------------------------------------------------------------------------------+ | |
// |  | | left-tab-headers-div      | |  | |  | | top-div                                                                              | | |
// |  | | +-----------------------+ | |  | |  | | +-------------------------------------+  +-+  +------------------------------------+ | | |
// |  | | | left-tab-header-div*  | | |  | |  | | | top-middle-tile-div                 |  |r|  | top-right-tile-div                 | | | |
// |  | | |                       | | |  | |  | | | +---------------------------------+ |  |i|  | +--------------------------------+ | | | |
// |  | | |                       | | |  | |  | | | | top-middle-tab-headers-div      | |  |g|  | | top-right-tab-headers-div      | | | | |
// |  | | |                       | | |  | |  | | | | +-----------------------------+ | |  |h|  | | +----------------------------+ | | | | |
// |  | | |                       | | |  | |  | | | | | top-middle-tab-header-div*  | | |  |t|  | | | top-right-tab-header-div*  | | | | | |
// |  | | |                       | | |  | |  | | | | |                             | | |  |-|  | | |                            | | | | | |
// |  | | +-----------------------+ | |  | |  | | | | +-----------------------------+ | |  |h|  | | +----------------------------+ | | | | |
// |  | +---------------------------+ |  | |  | | | +---------------------------------+ |  |o|  | +--------------------------------+ | | | |
// |  | +---------------------------+ |  | |  | | | +---------------------------------+ |  |r|  | +--------------------------------+ | | | |
// |  | | left-tab-contents-div     | |  |l|  | | | | top-middle-tab-contents-div     | |  |i|  | |   top-right-tab-contents-div   | | | | |
// |  | | +-----------------------+ | |  |e|  | | | | +-----------------------------+ | |  |z|  | | +----------------------------+ | | | | |
// |  | | | left-tab-content-div* | | |  |f|  | | | | | top-middle-tab-content-div* | | |  |-|  | | | top-right-tab-content-div* | | | | | |
// |  | | |                       | | |  |t|  | | | | |                             | | |  |r|  | | |                            | | | | | |
// |  | | |                       | | |  |-|  | | | | |                             | | |  |e|  | | |                            | | | | | |
// |  | | |                       | | |  |h|  | | | | |                             | | |  |s|  | | |                            | | | | | |
// |  | | |                       | | |  |o|  | | | | |                             | | |  |i|  | | |                            | | | | | |
// |  | | |                       | | |  |r|  | | | | |                             | | |  |z|  | | |                            | | | | | |
// |  | | |                       | | |  |i|  | | | | |                             | | |  |e|  | | |                            | | | | | |
// |  | | |                       | | |  |z|  | | | | |                             | | |  |r|  | | |                            | | | | | |
// |  | | |                       | | |  |-|  | | | | |                             | | |  |-|  | | |                            | | | | | |
// |  | | |                       | | |  |r|  | | | | |                             | | |  |d|  | | |                            | | | | | |
// |  | | |                       | | |  |e|  | | | | +-----------------------------+ | |  |i|  | | +----------------------------+ | | | | |
// |  | | |                       | | |  |s|  | | | +---------------------------------+ |  |v|  | +--------------------------------+ | | | |
// |  | | |                       | | |  |i|  | | +-------------------------------------+  +-+  +------------------------------------+ | | |
// |  | | |                       | | |  |z|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |e|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |r|  | |                              vert-resizer-div                                        | | |
// |  | | |                       | | |  |-|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |d|  | +--------------------------------------------------------------------------------------+ | |
// |  | | |                       | | |  |i|  | | bottom-tile-div                                                                      | | |
// |  | | |                       | | |  |v|  | | +----------------------------------------------------------------------------------+ | | |
// |  | | |                       | | |  | |  | | | bottom-tab-headers-div                                                           | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | |                       | | |  | |  | | | | bottom-tab-header-div*                                                       | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | |                       | | |  | |  | | +----------------------------------------------------------------------------------+ | | |
// |  | | |                       | | |  | |  | | +----------------------------------------------------------------------------------+ | | |
// |  | | |                       | | |  | |  | | | bottom-tab-contents-div                                                          | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | |                       | | |  | |  | | | | bottom-tab-content-div*                                                      | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | |                                                                              | | | | |
// |  | | |                       | | |  | |  | | | +------------------------------------------------------------------------------+ | | | |
// |  | | +-----------------------+ | |  | |  | | +----------------------------------------------------------------------------------+ | | |
// |  | +---------------------------+ |  | |  | +--------------------------------------------------------------------------------------+ | |
// |  +-------------------------------+  +-+  +------------------------------------------------------------------------------------------+ |
// +---------------------------------------------------------------------------------------------------------------------------------------+

GUI.prototype.min_tab_content_width = 64;  // minimum tab content width: enough to display horizontal scrollbar
GUI.prototype.min_tab_content_height = 64; // minimum tab content width: enough to display vertical scrollbar
GUI.prototype.window_resize_refresh_period = 500; // interface refresh period (in ms) while window is being resized
GUI.prototype.content_div = null;
GUI.prototype.left_tile_div = null;
GUI.prototype.left_tab_headers_div = null;
GUI.prototype.left_tab_header_div = null;
GUI.prototype.left_tab_contents_div = null;
GUI.prototype.left_tab_content_div = null;
GUI.prototype.left_horiz_resizer_div = null;
GUI.prototype.right_div = null;
GUI.prototype.top_div = null;
GUI.prototype.top_middle_tile_div = null;
GUI.prototype.top_middle_tab_headers_div = null;
GUI.prototype.top_middle_tab_header_div = null;
GUI.prototype.top_middle_tab_contents_div = null;
GUI.prototype.right_horiz_resizer_div = null;
GUI.prototype.top_right_tile_div = null;
GUI.prototype.top_right_tab_headers_div = null;
GUI.prototype.top_right_tab_header_div = null;
GUI.prototype.top_right_tab_contents_div = null;
GUI.prototype.vert_resizer_div = null;
GUI.prototype.bottom_tile_div = null;
GUI.prototype.bottom_tab_headers_div = null;
GUI.prototype.bottom_tab_header_div = null;
GUI.prototype.bottom_tab_contents_div = null;
GUI.prototype.left_tab_headers_history_shortcut = null;
GUI.prototype.left_tab_headers_left_scroller = null;
GUI.prototype.left_tab_headers_right_scroller = null;
GUI.prototype.top_middle_tab_headers_history_shortcut = null;
GUI.prototype.top_middle_tab_headers_left_scroller = null;
GUI.prototype.top_middle_tab_headers_right_scroller = null;
GUI.prototype.top_right_tab_headers_history_shortcut = null;
GUI.prototype.top_right_tab_headers_left_scroller = null;
GUI.prototype.top_right_tab_headers_right_scroller = null;
GUI.prototype.bottom_tab_headers_history_shortcut = null;
GUI.prototype.bottom_tab_headers_left_scroller = null;
GUI.prototype.bottom_tab_headers_right_scroller = null;
GUI.prototype.left_tabs = null;
GUI.prototype.top_middle_tabs = null;
GUI.prototype.top_right_tabs = null;
GUI.prototype.bottom_tabs = null;
GUI.prototype.mouse_pos = null;
GUI.prototype.bound_vert_resize = null;
GUI.prototype.bound_left_horiz_resize = null;
GUI.prototype.bound_right_horiz_resize = null;
GUI.prototype.window_inner_width = 0;
GUI.prototype.window_inner_height = 0;

function GUI()
{
	var magic = sessionStorage.getItem('gui.magic');
	if(!magic || (magic != 0xCAFE))
	{
		sessionStorage.clear();
	}
	
	this.content_div = document.getElementById('content-div');
	this.left_tile_div = document.getElementById('left-tile-div');
	this.left_tab_headers_div = document.getElementById('left-tab-headers-div');
	this.left_tab_header_div = document.getElementsByClassName('left-tab-header-div');
	this.left_tab_contents_div = document.getElementById('left-tab-contents-div');
	this.left_tab_content_div = document.getElementsByClassName('left-tab-content-div');
	this.left_horiz_resizer_div = document.getElementById('left-horiz-resizer-div');
	this.right_div = document.getElementById('right-div');
	this.top_div = document.getElementById('top-div');
	this.top_middle_tile_div = document.getElementById('top-middle-tile-div');
	this.top_middle_tab_headers_div = document.getElementById('top-middle-tab-headers-div');
	this.top_middle_tab_header_div = document.getElementsByClassName('top-middle-tab-header-div');
	this.top_middle_tab_contents_div = document.getElementById('top-middle-tab-contents-div');
	this.right_horiz_resizer_div = document.getElementById('right-horiz-resizer-div');
	this.top_right_tile_div = document.getElementById('top-right-tile-div');
	this.top_right_tab_headers_div = document.getElementById('top-right-tab-headers-div');
	this.top_right_tab_header_div = document.getElementsByClassName('top-right-tab-header-div');
	this.top_right_tab_contents_div = document.getElementById('top-right-tab-contents-div');
	this.vert_resizer_div = document.getElementById('vert-resizer-div');
	this.bottom_tile_div = document.getElementById('bottom-tile-div');
	this.bottom_tab_headers_div = document.getElementById('bottom-tab-headers-div');
	this.bottom_tab_header_div = document.getElementsByClassName('bottom-tab-header-div');
	this.bottom_tab_contents_div = document.getElementById('bottom-tab-contents-div');
	this.mouse_pos = new Point(0, 0);

	this.left_tabs = new Tabs(this, 'left-tabs', this.left_tile_div);
	this.top_middle_tabs = new Tabs(this, 'top-middle-tabs', this.top_middle_tile_div);
	this.top_right_tabs = new Tabs(this, 'top-right-tabs', this.top_right_tile_div);
	this.bottom_tabs = new Tabs(this, 'bottom-tabs', this.bottom_tile_div);

	this.vert_resizer_div.addEventListener('mousedown', this.vert_resizer_on_mousedown.bind(this));
	this.left_horiz_resizer_div.addEventListener('mousedown', this.left_horiz_resizer_on_mousedown.bind(this));
	this.right_horiz_resizer_div.addEventListener('mousedown', this.right_horiz_resizer_on_mousedown.bind(this));

	document.addEventListener('mouseup', this.document_on_mouseup.bind(this));
	
	this.restore_layout();
	this.window_inner_width = window.innerWidth;
	this.window_inner_height = window.innerHeight;
	this.resize_content(this.window_inner_width, this.window_inner_height);
	
	setInterval(this.resize.bind(this), this.window_resize_refresh_period);
	
	if(!magic || (magic != 0xCAFE))
	{
		this.create_left_tab('Documentation', 'documentation');
		this.create_top_right_tab('Registers', 'registers', '/registers').switch_to();
		this.create_top_right_tab('Memory', 'memory', '/memory');
		this.create_bottom_tab('Configuration', 'config', '/config').switch_to();
		this.create_bottom_tab('Statistics', 'stats', '/stats');
		this.create_bottom_tab('Log', 'log', '/log');
		this.left_tabs.switch_to_nth_tab(0);
	}
	
	sessionStorage.setItem('gui.magic', 0xCAFE);
}

GUI.prototype.create_left_tab = function(label, name, src)
{
	return this.left_tabs.create_tab(label, name, src);
}

GUI.prototype.create_top_middle_tab = function(label, name, src, object_name)
{
	return this.top_middle_tabs.create_tab(label, name, src, object_name);
}

GUI.prototype.create_top_right_tab = function(label, name, src)
{
	return this.top_right_tabs.create_tab(label, name, src);
}

GUI.prototype.create_bottom_tab = function(label, name, src)
{
	return this.bottom_tabs.create_tab(label, name, src);
}

GUI.prototype.open_object = function(object_url, object_name)
{
	var tab = this.top_middle_tabs.find_tab_by_object_name(object_name);
	if(!tab)
	{
		tab = this.create_top_middle_tab(object_name, object_name, object_url, object_name);
	}
	
	tab.switch_to();
	this.top_right_tabs.bind_to_object(object_name);
	this.bottom_tabs.bind_to_object(object_name);
}

GUI.prototype.on_tab_header_switch = function(tab)
{
	if(tab.owner == this.top_middle_tabs)
	{
		if(tab.tab_config.object_name)
		{
			this.top_right_tabs.bind_to_object(tab.tab_config.object_name);
			this.bottom_tabs.bind_to_object(tab.tab_config.object_name);
		}
		else
		{
			this.top_right_tabs.bind_to_object(null);
			this.bottom_tabs.bind_to_object(null);
		}
	}
}

GUI.prototype.enable_pointer_events = function(v)
{
	this.left_tabs.enable_pointer_events(v);
	this.top_middle_tabs.enable_pointer_events(v);
	this.top_right_tabs.enable_pointer_events(v);
	this.bottom_tabs.enable_pointer_events(v);
}

GUI.prototype.vert_resizer_on_mousedown = function(e)
{
	this.mouse_pos.y = e.screenY;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_pointer_events(false);
	this.bound_vert_resize = this.vert_resize.bind(this);
	document.addEventListener('mousemove', this.bound_vert_resize);
}

GUI.prototype.left_horiz_resizer_on_mousedown = function(e)
{
	this.mouse_pos.x = e.screenX;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_pointer_events(false);
	this.bound_left_horiz_resize = this.left_horiz_resize.bind(this);
	document.addEventListener('mousemove', this.bound_left_horiz_resize);
}

GUI.prototype.right_horiz_resizer_on_mousedown = function(e)
{
	this.mouse_pos.x = e.screenX;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_pointer_events(false);
	this.bound_right_horiz_resize = this.right_horiz_resize.bind(this);
	document.addEventListener('mousemove', this.bound_right_horiz_resize);
}

GUI.prototype.document_on_mouseup = function(e)
{
	if(this.bound_vert_resize != null)
	{
		document.removeEventListener('mousemove', this.bound_vert_resize);
		this.content_div.classList.remove('noselect');
		this.enable_pointer_events();
		this.bound_vert_resize = null;
	}
	if(this.bound_left_horiz_resize != null)
	{
		document.removeEventListener('mousemove', this.bound_left_horiz_resize);
		this.content_div.classList.remove('noselect');
		this.enable_pointer_events();
		this.bound_left_horiz_resize = null;
	}
	if(this.bound_right_horiz_resize != null)
	{
		document.removeEventListener('mousemove', this.bound_right_horiz_resize);
		this.content_div.classList.remove('noselect');
		this.enable_pointer_events();
		this.bound_right_horiz_resize = null;
	}
}

GUI.prototype.vert_resize = function(e)
{
	var dy = e.screenY - this.mouse_pos.y;
	this.mouse_pos.y = e.screenY;
	
	var top_middle_tab_headers_div_height = getHeight(this.top_middle_tab_headers_div);
	var top_right_tab_headers_div_height = getHeight(this.top_right_tab_headers_div);
	var bottom_tab_headers_div_height = getHeight(this.bottom_tab_headers_div);
	
	var top_div_height = getHeight(this.top_div);
	var bottom_tile_div_height = getHeight(this.bottom_tile_div);
	
	top_div_height = top_div_height + dy;
	bottom_tile_div_height = bottom_tile_div_height - dy;
	var top_middle_tab_contents_div_height = top_div_height - top_middle_tab_headers_div_height;
	var top_right_tab_contents_div_height = top_div_height - top_right_tab_headers_div_height;
	var bottom_tab_contents_div_height = bottom_tile_div_height - bottom_tab_headers_div_height;
	
	if((top_middle_tab_contents_div_height >= this.min_tab_content_height) &&
	   (top_right_tab_contents_div_height >= this.min_tab_content_height) &&
	   (bottom_tab_contents_div_height >= this.min_tab_content_height))
	{
		this.top_div.style.height = top_div_height + 'px';
		this.top_middle_tile_div.style.height = top_div_height + 'px';
		this.top_middle_tab_contents_div.style.height = top_middle_tab_contents_div_height + 'px';
		this.right_horiz_resizer_div.style.height = top_div_height + 'px';
		this.top_right_tile_div.style.height = top_div_height + 'px';
		this.top_right_tab_contents_div.style.height = top_right_tab_contents_div_height + 'px';
		this.bottom_tile_div.style.height = bottom_tile_div_height + 'px';
		this.bottom_tab_contents_div.style.height = bottom_tab_contents_div_height + 'px';
		
		this.save_layout();
	}
}

GUI.prototype.left_horiz_resize = function(e)
{
	var dx = e.screenX - this.mouse_pos.x;
	this.mouse_pos.x = e.screenX;
	
	var left_tile_div_width = getWidth(this.left_tile_div);
	var right_div_width = getWidth(this.right_div);
	var top_middle_tile_div_width = getWidth(this.top_middle_tile_div);
	
	left_tile_div_width = left_tile_div_width + dx;
	right_div_width = right_div_width - dx;
	top_middle_tile_div_width = top_middle_tile_div_width - dx;
	
	if((left_tile_div_width >= this.min_tab_content_width) && (right_div_width >= this.min_tab_content_width) && (top_middle_tile_div_width >= this.min_tab_content_width))
	{
		this.left_tile_div.style.width = left_tile_div_width + 'px';
		this.left_tab_contents_div.style.width = left_tile_div_width + 'px';
		this.right_div.style.width = right_div_width + 'px';
		this.top_div.style.width = right_div_width + 'px';
		this.top_middle_tile_div.style.width = top_middle_tile_div_width + 'px';
		this.top_middle_tab_contents_div.style.width = top_middle_tile_div_width + 'px';
		this.vert_resizer_div.style.width = right_div_width + 'px';
		this.bottom_tile_div.style.width = right_div_width + 'px';
		this.bottom_tab_contents_div.style.width = right_div_width + 'px';
		
		this.save_layout();
	}
}

GUI.prototype.right_horiz_resize = function(e)
{
	var dx = e.screenX - this.mouse_pos.x;
	this.mouse_pos.x = e.screenX;
	
	var top_middle_tile_div_width = getWidth(this.top_middle_tile_div);
	var top_right_tile_div_width = getWidth(this.top_right_tile_div);
	
	top_middle_tile_div_width = top_middle_tile_div_width + dx;
	top_right_tile_div_width = top_right_tile_div_width - dx;
	
	if((top_middle_tile_div_width >= this.min_tab_content_width) && (top_right_tile_div_width >= this.min_tab_content_width))
	{
		this.top_middle_tile_div.style.width = top_middle_tile_div_width + 'px';
		this.top_middle_tab_contents_div.style.width = top_middle_tile_div_width + 'px';
		this.top_right_tile_div.style.width = top_right_tile_div_width + 'px';
		this.top_right_tab_contents_div.style.width = top_right_tile_div_width + 'px';
		
		this.save_layout();
	}
}

GUI.prototype.save_layout = function()
{
	sessionStorage.setItem('gui.left-tile-div.style.width', this.left_tile_div.style.width);
	sessionStorage.setItem('gui.right-div.style.width', this.right_div.style.width);
	sessionStorage.setItem('gui.top-middle-tile-div.style.width', this.top_middle_tile_div.style.width);
	sessionStorage.setItem('gui.top-right-tile-div.style.width', this.top_right_tile_div.style.width);
	sessionStorage.setItem('gui.bottom-tile-div.style.width', this.bottom_tile_div.style.width);
	sessionStorage.setItem('gui.top-div.style.height', this.top_div.style.height);
	sessionStorage.setItem('gui.bottom-tile-div.style.height', this.bottom_tile_div.style.height);
}

GUI.prototype.restore_layout = function()
{
	var left_tile_div_style_width = sessionStorage.getItem('gui.left-tile-div.style.width');
	var right_div_style_width = sessionStorage.getItem('gui.right-div.style.width');
	var top_middle_tile_div_style_width = sessionStorage.getItem('gui.top-middle-tile-div.style.width');
	var top_right_tile_div_style_width = sessionStorage.getItem('gui.top-right-tile-div.style.width');
	var bottom_tile_div_style_width = sessionStorage.getItem('gui.bottom-tile-div.style.width');
	var top_div_style_height = sessionStorage.getItem('gui.top-div.style.height');
	var bottom_tile_div_style_height = sessionStorage.getItem('gui.bottom-tile-div.style.height');
	
	if(left_tile_div_style_width !== null) this.left_tile_div.style.width = left_tile_div_style_width;
	if(right_div_style_width !== null) this.right_div.style.width = right_div_style_width;
	if(top_middle_tile_div_style_width !== null) this.top_middle_tile_div.style.width = top_middle_tile_div_style_width;
	if(top_right_tile_div_style_width !== null) this.top_right_tile_div.style.width = top_right_tile_div_style_width;
	if(bottom_tile_div_style_width !== null) this.bottom_tile_div.style.width = bottom_tile_div_style_width;
	if(top_div_style_height !== null) this.top_div.style.height = top_div_style_height;
	if(bottom_tile_div_style_height !== null) this.bottom_tile_div.style.height = bottom_tile_div_style_height;
}

GUI.prototype.resize_content = function(content_width, content_height)
{
	var left_horiz_resizer_div_width = getWidth(this.left_horiz_resizer_div);
	var right_horiz_resizer_div_width = getWidth(this.right_horiz_resizer_div);
	var left_tab_headers_div_height = getHeight(this.left_tab_headers_div);
	var top_div_height = getHeight(this.top_div);
	var top_middle_tab_headers_div_height = getHeight(this.top_middle_tab_headers_div);
	var top_right_tab_headers_div_height = getHeight(this.top_right_tab_headers_div);
	var vert_resizer_div_height = getHeight(this.vert_resizer_div);
	var bottom_tab_headers_div_height = getHeight(this.bottom_tab_headers_div);
	
	var left_tile_div_width = getWidth(this.left_tile_div);
	var right_div_width = getWidth(this.right_div);
	var top_middle_tile_div_width = getWidth(this.top_middle_tile_div);
	var top_right_tile_div_width = getWidth(this.top_right_tile_div);
	
	var min_content_width = this.min_tab_content_width + left_horiz_resizer_div_width + this.min_tab_content_width + right_horiz_resizer_div_width + this.min_tab_content_width;
	var min_content_height = Math.max(left_tab_headers_div_height + this.min_tab_content_height, Math.max(top_middle_tab_headers_div_height, top_right_tab_headers_div_height) + this.min_tab_content_height + vert_resizer_div_height + bottom_tab_headers_div_height + this.min_tab_content_height);
	
	if(content_width < min_content_width)
	{
		content_width = min_content_width;
	}
	
	if(content_height < min_content_height)
	{
		content_height = min_content_height;
	}
	
	top_right_tile_div_width = Math.max(this.min_tab_content_width, content_width - left_tile_div_width - left_horiz_resizer_div_width - top_middle_tile_div_width - right_horiz_resizer_div_width);
	top_middle_tile_div_width = Math.max(this.min_tab_content_width, content_width - left_tile_div_width - left_horiz_resizer_div_width - right_horiz_resizer_div_width - top_right_tile_div_width);
	bottom_tile_div_height = Math.max(this.min_tab_content_height + bottom_tab_headers_div_height, content_height - vert_resizer_div_height - top_div_height);
	top_div_height = Math.max(this.min_tab_content_height + Math.max(top_middle_tab_headers_div_height, top_right_tab_headers_div_height), content_height - vert_resizer_div_height - bottom_tile_div_height);
	
	var right_div_width = top_middle_tile_div_width + right_horiz_resizer_div_width + top_right_tile_div_width;
	var left_tile_div_width = Math.max(this.min_tab_content_width, content_width - left_horiz_resizer_div_width - right_div_width);

	var left_tab_contents_div_height = Math.max(this.min_tab_content_height, content_height - left_tab_headers_div_height);
	var top_middle_tab_contents_div_height = Math.max(this.min_tab_content_height, top_div_height - top_middle_tab_headers_div_height);
	var top_right_tab_contents_div_height = Math.max(this.min_tab_content_height, top_div_height - top_right_tab_headers_div_height);
	var bottom_tab_contents_div_height = Math.max(this.min_tab_content_height, bottom_tile_div_height - bottom_tab_headers_div_height);
	
	this.content_div.style.width = content_width + 'px';
	this.content_div.style.height = content_height + 'px';
	this.left_tile_div.style.width = left_tile_div_width + 'px';
	this.left_tile_div.style.height = content_height + 'px';
	this.left_tab_contents_div.style.width = left_tile_div_width + 'px';
	this.left_tab_contents_div.style.height = left_tab_contents_div_height + 'px';
	this.left_horiz_resizer_div.style.height = content_height + 'px';
	this.right_div.style.width = right_div_width + 'px';
	this.right_div.style.height = content_height + 'px';
	this.top_div.style.width = right_div_width + 'px';
	this.top_div.style.height = top_div_height + 'px';
	this.top_middle_tile_div.style.width = top_middle_tile_div_width + 'px';
	this.top_middle_tile_div.style.height = top_div_height + 'px';
	this.top_middle_tab_contents_div.style.width = top_middle_tile_div_width + 'px';
	this.top_middle_tab_contents_div.style.height = top_middle_tab_contents_div_height + 'px';
	this.right_horiz_resizer_div.style.height = top_div_height + 'px';
	this.top_right_tile_div.style.width = top_right_tile_div_width + 'px';
	this.top_right_tile_div.style.height = top_div_height + 'px';
	this.top_right_tab_contents_div.style.width = top_right_tile_div_width + 'px';
	this.top_right_tab_contents_div.style.height = top_right_tab_contents_div_height + 'px';
	this.vert_resizer_div.style.width = right_div_width + 'px';
	this.bottom_tile_div.style.width = right_div_width + 'px';
	this.bottom_tile_div.style.height = bottom_tile_div_height + 'px';
	this.bottom_tab_contents_div.style.width = right_div_width + 'px';
	this.bottom_tab_contents_div.style.height = bottom_tab_contents_div_height + 'px';
	
	this.save_layout();
}

GUI.prototype.resize = function()
{
	var new_window_inner_width = window.innerWidth;
	var new_window_inner_height = window.innerHeight;
	if((new_window_inner_width != this.window_inner_width) || (new_window_inner_height != this.window_inner_height))
	{
		this.window_inner_width = new_window_inner_width;
		this.window_inner_height = new_window_inner_height;
		this.resize_content(this.window_inner_width, this.window_inner_height);
	}
}

GUI.prototype.find_tab_by_name = function(name)
{
	return this.top_middle_tabs.find_tab_by_name(name) || this.top_right_tabs.find_tab_by_name(name) || this.bottom_tabs.find_tab_by_name(name);
}

GUI.prototype.refresh_active_tabs = function()
{
	this.top_middle_tabs.refresh_active_tabs();
	this.top_right_tabs.refresh_active_tabs();
	this.bottom_tabs.refresh_active_tabs();
}

var gui = null;
document.addEventListener('DOMContentLoaded', function(event) { gui = new GUI(); });
