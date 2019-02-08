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

// Point
Point.prototype.x = null;
Point.prototype.y = null;

function Point(x, y)
{
	this.x = x || 0;
	this.y = y || 0;
}

// CompatLayer
var CompatLayer =
{
	is_internet_explorer : !!navigator.userAgent.match(/Trident/g) || !!navigator.userAgent.match(/MSIE/g),
	
	has_svg : !!(typeof SVGRect !== undefined),

	get_element_width : function(el)
	{
		var style = getComputedStyle(el, null);
		var width = parseFloat(style.getPropertyValue('width'));
		if(this.is_internet_explorer)
		{
			var border_left_width = parseFloat(style.getPropertyValue('border-left-width'));
			var border_right_width = parseFloat(style.getPropertyValue('border-right-width'));
			
			return width + border_left_width + border_right_width;
		}
		
		return width;
	},

	get_element_height : function(el)
	{
		var style = getComputedStyle(el, null);
		var height = parseFloat(style.getPropertyValue('height'));
		if(this.is_internet_explorer)
		{
			var border_top_width = parseFloat(style.getPropertyValue('border-top-width'));
			var border_bottom_width = parseFloat(style.getPropertyValue('border-bottom-width'));
			
			return height + border_top_width + border_bottom_width;
		}
		
		return height;
	},

	get_iframe_scroll_pos : function(iframe)
	{
		return this.is_internet_explorer ? new Point(iframe.contentDocument.documentElement.scrollLeft, iframe.contentDocument.documentElement.scrollTop)
		                                 : new Point(iframe.contentWindow.scrollX, iframe.contentWindow.scrollY);
	},

	set_iframe_scroll_pos : function(iframe, scroll_pos)
	{
		if(this.is_internet_explorer)
		{
			iframe.contentDocument.documentElement.scrollLeft = scroll_pos.x;
			iframe.contentDocument.documentElement.scrollTop = scroll_pos.y;
		}
		else
		{
			iframe.contentWindow.scrollTo(scroll_pos.x, scroll_pos.y);
		}
	}
}

// IFrame
IFrame.prototype.name = null;
IFrame.prototype.src = null;
IFrame.prototype.class_name = null;
IFrame.prototype.storage_item_prefix = null;
IFrame.prototype.parent_element = null;
IFrame.prototype.visible = false;
IFrame.prototype.bg = 0;
IFrame.prototype.fg = 1;
IFrame.prototype.iframe_element = null;
IFrame.prototype.loaded = null;

function IFrame(name, src, class_name, storage_item_prefix, parent_element)
{
	this.name = name;
	this.src = src;
	this.class_name = class_name;
	this.storage_item_prefix = storage_item_prefix;
	this.visible = false;
	this.bg = 0;
	this.fg = 1;
	this.iframe_element = new Array(2);
	this.load_requested = new Array(2);
	this.loaded = new Array(2);
	for(var i = 0; i < 2; i++)
	{
		this.iframe_element[i] = document.createElement('iframe');
		this.iframe_element[i].className = this.class_name;
		this.iframe_element[i].setAttribute('name', this.name + '-' + i);
		this.iframe_element[i].setAttribute('id', this.name + '-' + i);
		this.load_requested[i] = false;
		this.loaded[i] = false;
		this.iframe_element[i].addEventListener('load', this.on_load.bind(this, i));
		this.iframe_element[i].addEventListener('mouseenter', this.on_mouseenter.bind(this));
		this.iframe_element[i].addEventListener('mouseleave', this.on_mouseleave.bind(this));
	}
	this.attach(parent_element);
}

IFrame.prototype.destroy = function()
{
	this.detach();
	for(var i = 0; i < 2; i++)
	{
		this.iframe_element[i] = null;
		this.loaded[i] = false;
	}
	this.name = null;
	this.src = null;
	this.class_name = null;
	this.storage_item_prefix = null;
	this.parent_element = null;
}

IFrame.prototype.on_load = function(i)
{
//	console.log(this.name + ':frame #' + i + ' loaded');
	this.bg = i;
	this.fg = 1 - this.bg;
	this.loaded[this.bg] = true;
	this.restore_scroll_position();
	this.iframe_element[this.bg].setAttribute('style', 'visibility:' + (this.visible ? 'visible' : 'hidden'));
	if(this.iframe_element[this.fg]) this.iframe_element[this.fg].setAttribute('style', 'visibility:hidden');
	this.fg = this.bg;
	this.bg = 1 - this.fg;
// 	console.log(this.name + ':next frame is frame #' + this.bg);
}

IFrame.prototype.refresh = function()
{
//	console.log(this.name + ':refresh');
	this.save_scroll_position();
	this.loaded[this.bg] = false;
	this.load_requested[this.bg] = true;
	this.iframe_element[this.bg].setAttribute('src', this.src);
}

IFrame.prototype.set_visible = function(v)
{
	this.visible = v;
	this.iframe_element[this.fg].style.visibility = (this.visible && this.loaded[this.fg]) ? 'visible' : 'hidden';
}

IFrame.prototype.on_mouseenter = function()
{
	for(var i = 0; i < 2; i++)
	{
		this.iframe_element[i].setAttribute('scrolling', 'yes');
		this.iframe_element[i].style.overflow = 'auto';
	}
}

IFrame.prototype.on_mouseleave = function()
{
	for(var i = 0; i < 2; i++)
	{
		this.iframe_element[i].setAttribute('scrolling', 'no');
		this.iframe_element[i].style.overflow = 'hidden';
	}
}

IFrame.prototype.enable_pointer_events = function(v)
{
	for(var i = 0; i < 2; i++)
	{
		this.iframe_element[i].style.pointerEvents = (((v === undefined) || v) ? 'auto' : 'none');
	}
}

IFrame.prototype.save_scroll_position = function()
{
	if(!this.loaded[this.fg]) return;
	
	if(!this.iframe_element[this.fg].contentWindow) return;
	
	if(!this.iframe_element[this.fg].contentDocument) return;
	
	var scroll_x_item_name = this.storage_item_prefix + 'scroll-x';
	var scroll_y_item_name = this.storage_item_prefix + 'scroll-y';
	
	var scroll_pos = CompatLayer.get_iframe_scroll_pos(this.iframe_element[this.fg]);
	
// 	console.log(this.name + ':' + scroll_x_item_name + ' <- ' + scroll_pos.x);
// 	console.log(this.name + ':' + scroll_y_item_name + ' <- ' + scroll_pos.y);
	
	sessionStorage.setItem(scroll_x_item_name, scroll_pos.x);
	sessionStorage.setItem(scroll_y_item_name, scroll_pos.y);
}

IFrame.prototype.restore_scroll_position = function()
{
	if(!this.loaded[this.bg]) return;
	
	if(!this.iframe_element[this.bg].contentWindow) return;
	
	if(!this.iframe_element[this.bg].contentDocument) return;

	var scroll_x_item_name = this.storage_item_prefix + 'scroll-x';
	var scroll_y_item_name = this.storage_item_prefix + 'scroll-y';
	
	var scroll_x = sessionStorage.getItem(scroll_x_item_name);
	var scroll_y = sessionStorage.getItem(scroll_y_item_name);
	
	if(scroll_x && scroll_y)
	{
// 		console.log(this.name + ':' + scroll_x_item_name + ' == ' + scroll_x);
// 		console.log(this.name + ':' + scroll_y_item_name + ' == ' + scroll_y);
		
		CompatLayer.set_iframe_scroll_pos(this.iframe_element[this.bg], new Point(scroll_x, scroll_y));
	}
}

IFrame.prototype.set_scroll_pos = function(scroll_pos)
{
	for(var i = 0; i < 2; i++)
	{
		CompatLayer.set_iframe_scroll_pos(this.iframe_element[i], scroll_pos);
	}
}

IFrame.prototype.has_iframe = function(iframe_name)
{
	for(var i = 0; i < 2; i++)
	{
		if(this.iframe_element[i].hasAttribute('name') && (this.iframe_element[i].getAttribute('name') == iframe_name)) return true;
	}
	
	return false;
}

IFrame.prototype.get_next_target = function(iframe_name)
{
	if(this.iframe_element[0].hasAttribute('name') && this.iframe_element[1].hasAttribute('name'))
	{
		if(this.iframe_element[0].getAttribute('name') == iframe_name)
		{
			return this.iframe_element[1].getAttribute('name');
		}
		else if(this.iframe_element[1].getAttribute('name') == iframe_name)
		{
			return this.iframe_element[0].getAttribute('name');
		}
	}
	
	return '_self';
}

IFrame.prototype.detach = function()
{
	this.save_scroll_position();
	for(var i = 0; i < 2; i++)
	{
		this.parent_element.removeChild(this.iframe_element[i]);
		this.loaded[i] = false;
	}
	this.parent_element = null;
}

IFrame.prototype.attach = function(parent_element)
{
	this.parent_element = parent_element;
	for(var i = 0; i < 2; i++)
	{
		this.parent_element.appendChild(this.iframe_element[i]);
	}
	this.restore_scroll_position();
}

// TabConfig
TabConfig.prototype.label = null;
TabConfig.prototype.name = null;
TabConfig.prototype.src = null;

function TabConfig(label, name, src)
{
	this.label = label;
	this.name = name;
	this.src = src;
}

TabConfig.prototype.save = function(prefix)
{
	if(this.label) sessionStorage.setItem(prefix + 'label', this.label);
	if(this.name) sessionStorage.setItem(prefix + 'name', this.name);
	if(this.src) sessionStorage.setItem(prefix + 'src', this.src);
}

TabConfig.prototype.restore = function(prefix)
{
	this.label = sessionStorage.getItem(prefix + 'label');
	this.name = sessionStorage.getItem(prefix + 'name');
	this.src = sessionStorage.getItem(prefix + 'src');
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
	sessionStorage.setItem(prefix + 'length', this.tab_configs.length);
	this.tab_configs.forEach(function(tab_config, index) { tab_config.save(prefix + 'tab' + index + '.'); });
}

TabConfigs.prototype.restore = function(prefix)
{
	var length = sessionStorage.getItem(prefix + 'length') || 0;
	for(var i = 0; i < length; i++)
	{
		var tab_config = new TabConfig();
		tab_config.restore(prefix + 'tab' + i + '.');
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
Tab.prototype.owner = null;
Tab.prototype.tab_config = null;
Tab.prototype.static = false;
Tab.prototype.tab_header = null;
Tab.prototype.tab_content = null;
Tab.prototype.close_tab = null;
Tab.prototype.bound_close_tab_onclick = null;
Tab.prototype.is_active = false;
Tab.prototype.loaded = false;
Tab.prototype.bound_tab_header_onclick = null;
Tab.prototype.bound_tab_header_oncontextmenu = null;
Tab.prototype.storage_item_prefix = null;

function Tab(owner)
{
	this.owner = owner;
	this.tab_config = null;
	this.static = false;
	this.tab_header = null;
	this.tab_content = null;
	this.close_tab = null;
	this.bound_close_tab_onclick = null;
	this.is_active = false;
	this.loaded = false;
	this.bound_tab_header_onclick = null;
	this.bound_tab_header_oncontextmenu = null;
	this.storage_item_prefix = null;
}

Tab.prototype.create = function(tab_config)
{
	this.static = false;
	this.tab_config = tab_config;
	this.close_tab = document.createElement('div');
	this.close_tab.className = 'close-tab ' + (CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tab_header = document.createElement('div');
	this.tab_header.className = 'tab-header noselect';
	this.tab_header.innerHTML = this.tab_config.label;
	this.tab_header.appendChild(this.close_tab);
	this.owner.tab_headers.appendChild(this.tab_header);
	this.storage_item_prefix = 'gui.' + this.owner.name + '.' + this.tab_config.name + '.';
	this.tab_content = new IFrame(this.tab_config.name, this.tab_config.src, 'tab-content', this.storage_item_prefix, this.owner.tab_contents);
	
	this.bound_tab_header_onclick = this.tab_header_onclick.bind(this);
	this.tab_header.addEventListener('click', this.bound_tab_header_onclick, false);
	
	this.bound_tab_header_oncontextmenu = this.tab_header_oncontextmenu.bind(this);
	this.tab_header.addEventListener('contextmenu', this.bound_tab_header_oncontextmenu);
	
	this.bound_close_tab_onclick = this.close_tab_onclick.bind(this);
	this.close_tab.addEventListener('click', this.bound_close_tab_onclick);
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

	this.close_tab = null;
	this.bound_close_tab_onclick = null;
}

Tab.prototype.destroy = function()
{
	if(!this.static)
	{
		this.owner.tab_headers.removeChild(this.tab_header);
		this.tab_content.destroy();
		this.tab_content = null;
		this.tab_header.removeEventListener('click', this.bound_tab_header_onclick);
		if(this.close_tab)
		{
			this.close_tab.removeEventListener('click', this.bound_close_tab_onclick);
		}
		this.owner = null;
		this.tab_config = null;
		this.tab_header = null;
		this.tab_content = null;
		this.close_tab = null;
		this.bound_close_tab_onclick = null;
		this.bound_tab_header_onclick = null;
	}
}

Tab.prototype.detach = function()
{
	this.owner.detach_tab(this);
}

Tab.prototype.attach = function(to)
{
	to.attach_tab(this);
}

Tab.prototype.save = function(prefix)
{
	this.tab_config.save(prefix);
}

Tab.prototype.restore = function(prefix)
{
	this.tab_config.restore(prefix);
}

Tab.prototype.refresh = function()
{
	if(!this.static)
	{
		this.tab_content.refresh();
	}
}

Tab.prototype.tab_content_on_load = function(event)
{
	this.loaded = true;
	this.restore_scroll_position();
}

Tab.prototype.save_scroll_position = function()
{
	if(!this.static)
	{
		this.tab_content.save_scroll_position();
	}
}

Tab.prototype.restore_scroll_position = function()
{
	if(!this.static)
	{
		this.tab_content.restore_scroll_position();
	}
}

Tab.prototype.set_active = function(v)
{
	if(this.is_active != v)
	{
		if(v)
		{
			this.tab_header.classList.add('tab-header-active');
		}
		else
		{
			this.tab_header.classList.remove('tab-header-active');
		}
		if(this.static)
		{
			this.tab_content.style.visibility = v ? 'visible' : 'hidden';
		}
		else
		{
			this.tab_content.set_visible(v);
		}
		this.is_active = v;
	}
}

Tab.prototype.enable_pointer_events = function(v)
{
	if(!this.static)
	{
		this.tab_content.enable_pointer_events(v);
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

Tab.prototype.tab_header_oncontextmenu = function(event)
{
	var context_menu_items = [
	{
		label : 'Close',
		action : function(event)
		{
			this.close();
		}
	},
	{
		label : 'Move to new window',
		action : function(event)
		{
			gui.move_tab_to_new_window(this);
		}
	} ];
	
	if(this.owner.name != 'left-tabs')
	{
		context_menu_items.push(
		{
			label : 'Move to left',
			action : function(event)
			{
				gui.move_tab_to_tile(this, 'left-tile');
			}
		});
	}
	
	if(this.owner.name != 'top-middle-tabs')
	{
		context_menu_items.push(
		{
			label : 'Move to center',
			action : function(event)
			{
				gui.move_tab_to_tile(this, 'top-middle-tile');
			}
		});
	}
	
	if(this.owner.name != 'top-right-tabs')
	{
		context_menu_items.push(
		{
			label : 'Move to right',
			action : function(event)
			{
				gui.move_tab_to_tile(this, 'top-right-tile');
			}
		});
	}

	if(this.owner.name != 'bottom-tabs')
	{
		context_menu_items.push(
		{
			label : 'Move to bottom',
			action : function(event)
			{
				gui.move_tab_to_tile(this, 'bottom-tile');
			}
		});
	}

	this.owner.owner.create_context_menu(event, context_menu_items, this);
}

Tab.prototype.close_tab_onclick = function(event)
{
	this.close();
	event.stopImmediatePropagation(); // avoid calling of this.tab_header_onclick after tab destruction
}

Tab.prototype.close = function()
{
	if(!this.static)
	{
		this.owner.destroy_tab(this);
	}
}

Tab.prototype.has_iframe = function(iframe_name)
{
	return !this.static && this.tab_content.has_iframe(iframe_name);
}

Tab.prototype.get_next_target = function(iframe_name)
{
	return this.static ? '_self' : this.tab_content.get_next_target(iframe_name);
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
	this.history_shortcut_element.classList.add(CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tab_headers_left_scroller = this.tile.getElementsByClassName('tab-headers-left-scroller')[0];
	this.tab_headers_left_scroller.classList.add(CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tab_headers_right_scroller = this.tile.getElementsByClassName('tab-headers-right-scroller')[0];
	this.tab_headers_right_scroller.classList.add(CompatLayer.has_svg ? 'w-svg' : 'wo-svg');
	this.tabs = new Array();
	this.tab_config_history = new TabConfigs();
	this.history_element = null;
	var saved_tab_configs = new TabConfigs();
	saved_tab_configs.restore('gui.' + this.name + '.');
	
	var history_shortcut_element_width = CompatLayer.get_element_width(this.history_shortcut_element);
	var left_scroller_width = CompatLayer.get_element_width(this.tab_headers_left_scroller);
	var right_scroller_width = CompatLayer.get_element_width(this.tab_headers_right_scroller);
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
		tab.save('gui.' + this.name + '.tab' + i + '.');
	}
}

Tabs.prototype.save_history = function()
{
	this.tab_config_history.save('gui.' + this.name + '.history' + '.');
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
	this.tab_config_history.restore('gui.' + this.name + '.history' + '.');
}

Tabs.prototype.restore_tab_headers_left = function()
{
	var tab_headers_left = sessionStorage.getItem('gui.' + this.name + '.tab-headers.style.left');
	if(tab_headers_left) this.tab_headers.style.left = tab_headers_left;
}

Tabs.prototype.compute_dynamic_widths = function()
{
	this.tile_width = this.tile_width = CompatLayer.get_element_width(this.tile);
	this.tab_headers_width = 0;
	var tab_headers = this.tab_headers.getElementsByClassName('tab-header');
	for(var i = 0; i < tab_headers.length; i++)
	{
		var tab_header = tab_headers[i];
		var tab_header_width = CompatLayer.get_element_width(tab_header);
		this.tab_headers_width = this.tab_headers_width + tab_header_width;
	}
}

Tabs.prototype.create_tab = function(label, name, src)
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
			this.tab_config_history.remove(tab_config);
			this.save_history();
		}
		else
		{
			tab_config = new TabConfig(label, name, src)
		}
		tab.create(tab_config);
		this.tabs.push(tab);
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
		var tab = this.tabs[tab_num];
		tab.switch_to();
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

Tabs.prototype.find_tab_by_iframe_name = function(iframe_name)
{
	return this.tabs.find(function(elt) { return elt.has_iframe(iframe_name); });
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

Tabs.prototype.detach_tab = function(tab_to_detach)
{
	var tab_num = this.tabs.findIndex(function(tab) { return tab == tab_to_detach; });
	
	if(tab_num >= 0)
	{
		var is_active = tab_to_detach.is_active;
		this.tabs.splice(tab_num, 1);
		this.save_tabs();
		this.save_history();
		if(is_active && (this.tabs.length != 0))
		{
			this.switch_to_nth_tab((tab_num > 0) ? (tab_num - 1) : tab_num);
		}
		this.tab_headers.removeChild(tab_to_detach.tab_header);
		if(this.static)
		{
			this.tab_contents.removeChild(tab_to_detach.tab_content);
		}
		else
		{
			tab_to_detach.tab_content.detach();
		}
		tab_to_detach.owner = null;
	}
}

Tabs.prototype.attach_tab = function(tab)
{
	if(tab.owner != this)
	{
		tab.owner = this;
		this.tab_headers.appendChild(tab.tab_header);
		if(this.static)
		{
			this.tab_contents.appendChild(tab.tab_content);
		}
		else
		{
			tab.tab_content.attach(this.tab_contents);
		}
		this.tabs.push(tab);
		this.save_tabs();
	}
}

Tabs.prototype.for_each_tab = function(callback)
{
	this.tabs.forEach(callback);
}

// ContextMenu
ContextMenu.action_this = null;
ContextMenu.context_menu_element = null;

function ContextMenu(event, context_menu_items, action_this)
{
	this.action_this = action_this;
	this.context_menu_element = document.createElement('div');
	this.context_menu_element.className = 'context-menu';
	this.context_menu_element.style.display = 'block';
	this.context_menu_element.style.position = 'fixed';
	this.context_menu_element.style.left = event.clientX + 'px';
	this.context_menu_element.style.top = event.clientY + 'px';
	
	for(var i = 0; i < context_menu_items.length; i++)
	{
		var context_menu_item_element = document.createElement('div');
		context_menu_item_element.className = 'context-menu-item';
		context_menu_item_element.innerHTML = context_menu_items[i].label;
		if(this.action_this)
		{
			context_menu_item_element.addEventListener('click', context_menu_items[i].action.bind(this.action_this));
		}
		else
		{
			context_menu_item_element.addEventListener('click', context_menu_items[i].action);
		}
		this.context_menu_element.appendChild(context_menu_item_element);
	}
	
	document.body.appendChild(this.context_menu_element);
	
	var window_inner_height = window.innerHeight;
	var style = getComputedStyle(this.context_menu_element, null);
	var top = parseFloat(style.getPropertyValue('top'));
	var height = parseFloat(style.getPropertyValue('height'));
	if((top + height) >= window_inner_height)
	{
		this.context_menu_element.style.top = (window_inner_height - height) + 'px';
	}

	event.preventDefault();
}

ContextMenu.prototype.destroy = function()
{
	document.body.removeChild(this.context_menu_element);
	this.action_this = null;
	this.context_menu_element = null;
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

GUI.prototype.magic = 0xCAFE;
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
GUI.prototype.context_menu = null;
GUI.prototype.child_windows_monitoring_period = 500;
GUI.prototype.child_windows = null;

function GUI()
{
	var magic = sessionStorage.getItem('gui.magic');
	if(!magic || (magic != this.magic))
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
	this.bound_vert_resize = null;
	this.bound_left_horiz_resize = null;
	this.bound_right_horiz_resize = null;
	this.window_inner_width = 0;
	this.window_inner_height = 0;
	this.context_menu = null;
	this.child_windows = new Array();

	this.left_tabs = new Tabs(this, 'left-tabs', this.left_tile_div);
	this.top_middle_tabs = new Tabs(this, 'top-middle-tabs', this.top_middle_tile_div);
	this.top_right_tabs = new Tabs(this, 'top-right-tabs', this.top_right_tile_div);
	this.bottom_tabs = new Tabs(this, 'bottom-tabs', this.bottom_tile_div);

	this.vert_resizer_div.addEventListener('mousedown', this.vert_resizer_on_mousedown.bind(this));
	this.left_horiz_resizer_div.addEventListener('mousedown', this.left_horiz_resizer_on_mousedown.bind(this));
	this.right_horiz_resizer_div.addEventListener('mousedown', this.right_horiz_resizer_on_mousedown.bind(this));

	document.addEventListener('mouseup', this.document_on_mouseup.bind(this));
	document.addEventListener('click', this.destroy_context_menu.bind(this));
	
	this.restore_layout();
	this.window_inner_width = window.innerWidth;
	this.window_inner_height = window.innerHeight;
	this.resize_content(this.window_inner_width, this.window_inner_height);
	
	setInterval(this.resize.bind(this), this.window_resize_refresh_period);
	
	if(!magic || (magic != this.magic))
	{
		this.left_tabs.switch_to_nth_tab(0);
	}
	
	sessionStorage.setItem('gui.magic', this.magic);
	
	setInterval(this.monitor_child_windows.bind(this), this.child_windows_monitoring_period);
}

GUI.prototype.open_tab = function(tile_name, label, name, src)
{
	var tab = this.find_tab_by_name(name);
	
	if(!tab)
	{
		switch(tile_name)
		{
			case 'left-tile':
				tab = this.left_tabs.create_tab(label, name, src);
				break;
			case 'top-middle-tile':
				tab = this.top_middle_tabs.create_tab(label, name, src);
				break;
			case 'top-right-tile':
				tab = this.top_right_tabs.create_tab(label, name, src);
				break;
			case 'bottom-tile':
				tab = this.bottom_tabs.create_tab(label, name, src);
				break;
		}
	}
	
	if(tab)
	{
		tab.switch_to();
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
	this.mouse_pos.y = window.devicePixelRatio ? Math.round(e.screenY / window.devicePixelRatio) : e.screenY;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_pointer_events(false);
	if(this.bound_vert_resize == null)
	{
		this.bound_vert_resize = this.vert_resize.bind(this);
		document.addEventListener('mousemove', this.bound_vert_resize);
	}
}

GUI.prototype.left_horiz_resizer_on_mousedown = function(e)
{
	this.mouse_pos.x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_pointer_events(false);
	if(this.bound_left_horiz_resize == null)
	{
		this.bound_left_horiz_resize = this.left_horiz_resize.bind(this);
		document.addEventListener('mousemove', this.bound_left_horiz_resize);
	}
}

GUI.prototype.right_horiz_resizer_on_mousedown = function(e)
{
	this.mouse_pos.x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	this.content_div.classList.add('noselect'); // prevent web browser from selecting text/image while resizing interface
	this.enable_pointer_events(false);
	if(this.bound_right_horiz_resize == null)
	{
		this.bound_right_horiz_resize = this.right_horiz_resize.bind(this);
		document.addEventListener('mousemove', this.bound_right_horiz_resize);
	}
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
	var y = window.devicePixelRatio ? Math.round(e.screenY / window.devicePixelRatio) : e.screenY;
	var dy = y - this.mouse_pos.y;
	this.mouse_pos.y = y;
	
	var top_middle_tab_headers_div_height = CompatLayer.get_element_height(this.top_middle_tab_headers_div);
	var top_right_tab_headers_div_height = CompatLayer.get_element_height(this.top_right_tab_headers_div);
	var bottom_tab_headers_div_height = CompatLayer.get_element_height(this.bottom_tab_headers_div);
	
	var top_div_height = CompatLayer.get_element_height(this.top_div);
	var bottom_tile_div_height = CompatLayer.get_element_height(this.bottom_tile_div);
	
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
	var x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	var dx = x - this.mouse_pos.x;
	this.mouse_pos.x = x;
	
	var left_tile_div_width = CompatLayer.get_element_width(this.left_tile_div);
	var right_div_width = CompatLayer.get_element_width(this.right_div);
	var top_middle_tile_div_width = CompatLayer.get_element_width(this.top_middle_tile_div);
	
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
	var x = window.devicePixelRatio ? Math.round(e.screenX / window.devicePixelRatio) : e.screenX;
	var dx = x - this.mouse_pos.x;
	this.mouse_pos.x = x;
	
	var top_middle_tile_div_width = CompatLayer.get_element_width(this.top_middle_tile_div);
	var top_right_tile_div_width = CompatLayer.get_element_width(this.top_right_tile_div);
	
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
	var left_horiz_resizer_div_width = CompatLayer.get_element_width(this.left_horiz_resizer_div);
	var right_horiz_resizer_div_width = CompatLayer.get_element_width(this.right_horiz_resizer_div);
	var left_tab_headers_div_height = CompatLayer.get_element_height(this.left_tab_headers_div);
	var top_div_height = CompatLayer.get_element_height(this.top_div);
	var top_middle_tab_headers_div_height = CompatLayer.get_element_height(this.top_middle_tab_headers_div);
	var top_right_tab_headers_div_height = CompatLayer.get_element_height(this.top_right_tab_headers_div);
	var vert_resizer_div_height = CompatLayer.get_element_height(this.vert_resizer_div);
	var bottom_tab_headers_div_height = CompatLayer.get_element_height(this.bottom_tab_headers_div);
	
	var left_tile_div_width = CompatLayer.get_element_width(this.left_tile_div);
	var right_div_width = CompatLayer.get_element_width(this.right_div);
	var top_middle_tile_div_width = CompatLayer.get_element_width(this.top_middle_tile_div);
	var top_right_tile_div_width = CompatLayer.get_element_width(this.top_right_tile_div);
	
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
	return this.left_tabs.find_tab_by_name(name) || this.top_middle_tabs.find_tab_by_name(name) || this.top_right_tabs.find_tab_by_name(name) || this.bottom_tabs.find_tab_by_name(name);
}

GUI.prototype.find_tab_by_iframe_name = function(iframe_name)
{
	return this.left_tabs.find_tab_by_iframe_name(iframe_name) || this.top_middle_tabs.find_tab_by_iframe_name(iframe_name) || this.top_right_tabs.find_tab_by_iframe_name(iframe_name) || this.bottom_tabs.find_tab_by_iframe_name(iframe_name);
}

GUI.prototype.create_context_menu = function(event, context_menu_items, action_this)
{
	this.destroy_context_menu();
	this.context_menu = new ContextMenu(event, context_menu_items, action_this);
	this.enable_pointer_events(false);
}

GUI.prototype.destroy_context_menu = function()
{
	if(this.context_menu)
	{
		this.context_menu.destroy();
		this.context_menu = null;
		this.enable_pointer_events(true);
	}
}

GUI.prototype.move_tab_to_tile = function(tab, tile_name)
{
	var destination = null;
	switch(tile_name)
	{
		case 'left-tile':
			destination = this.left_tabs;
			break;
		case 'top-middle-tile':
			destination = this.top_middle_tabs;
			break;
		case 'top-right-tile':
			destination = this.top_right_tabs;
			break;
		case 'bottom-tile':
			destination = this.bottom_tabs;
			break;
	}
	
	if(destination)
	{
		tab.detach();
		tab.attach(destination);
		tab.switch_to();
	}
}

GUI.prototype.move_tab_to_new_window = function(tab)
{
	tab.detach();
	var child_window = window.open(tab.tab_config.src);
	if(child_window)
	{
		this.child_windows.push(child_window);
	}
}

GUI.prototype.for_each_tab = function(callback)
{
	this.left_tabs.for_each_tab(callback);
	this.top_middle_tabs.for_each_tab(callback);
	this.top_right_tabs.for_each_tab(callback);
	this.bottom_tabs.for_each_tab(callback);
}

GUI.prototype.for_each_child_window = function(callback)
{
	this.child_windows.forEach(function(child_window) { if(!child_window.closed) callback(child_window); } );
}

GUI.prototype.monitor_child_windows = function()
{
	var i = 0;
	while(i < this.child_windows.length)
	{
		var child_window = this.child_windows[i];
		
		if(child_window.closed)
		{
			this.child_windows.splice(i, 1);
		}
		else
		{
			i++;
		}
	}
}

var gui = null;
document.addEventListener('DOMContentLoaded', function(event) { gui = new GUI(); });
