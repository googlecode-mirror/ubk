/**
 * http://www.openjs.com/scripts/events/keyboard_shortcuts/
 * Version : 2.01.A
 * By Binny V A
 * License : BSD
 */
var CShortcut = Class.create({
	initialize: function() {
		this.all = {};
		//Work around for stupid Shift key bug created by using lowercase - as a result the shift+num combination was broken
		this.shiftNums = {
			"`":"~",
			"1":"!",
			"2":"@",
			"3":"#",
			"4":"$",
			"5":"%",
			"6":"^",
			"7":"&",
			"8":"*",
			"9":"(",
			"0":")",
			"-":"_",
			"=":"+",
			";":":",
			"'":"\"",
			",":"<",
			".":">",
			"/":"?",
			"\\":"|"
		};
		//Special Keys - and their codes
		this.specialKeys = {
			'esc':27,
			'escape':27,
			'tab':9,
			'space':32,
			'return':13,
			'enter':13,
			'backspace':8,

			'scrolllock':145,
			'scroll_lock':145,
			'scroll':145,
			'capslock':20,
			'caps_lock':20,
			'caps':20,
			'numlock':144,
			'num_lock':144,
			'num':144,
			
			'pause':19,
			'break':19,
			
			'insert':45,
			'home':36,
			'delete':46,
			'end':35,
			
			'pageup':33,
			'page_up':33,
			'pu':33,

			'pagedown':34,
			'page_down':34,
			'pd':34,

			'left':37,
			'up':38,
			'right':39,
			'down':40,

			'f1':112,
			'f2':113,
			'f3':114,
			'f4':115,
			'f5':116,
			'f6':117,
			'f7':118,
			'f8':119,
			'f9':120,
			'f10':121,
			'f11':122,
			'f12':123
		};

	},

	//The function to be called at keypress
	handler: function(event) {
		
		var element = event.element();
		//Don't enable shortcut keys in Input, Textarea fields
		if (opt.disableInFields) { 
			if (element.tagName == 'INPUT' || element.tagName == 'TEXTAREA') return;
		}

		//Find Which key is pressed
		var code = event.keyCode || event.which;
		var character = String.fromCharCode(code).toLowerCase();
		
		if(code == 188) character=","; //If the user presses , when the type is onkeydown
		if(code == 190) character="."; //If the user presses . when the type is onkeydown

		var keys = this.shortcut.split("+");

		//Key Pressed - counts the number of valid keypresses - if it is same as the number of keys, the shortcut function is invoked
		var kp = 0;
		var modifiers = { 
			shift : { wanted:keys.indexOf('shift') != -1, pressed: event.shiftKey }
			,ctrl : { wanted:keys.indexOf('ctrl') != -1, pressed: event.ctrlKey }
			,alt  : { wanted:keys.indexOf('alt') != -1, pressed: event.altKey }
			,meta : { wanted:keys.indexOf('meta') != -1, pressed: event.metaKey }
		};
					
		for(var i=0; k=keys[i],i<keys.length; i++) {
			//Modifiers
			if (['ctrl','shift','alt','meta'].indexOf(k) != -1 ) {
			
				kp++;

			} else if (k.length > 1) { //If it is a special key

				if (this.object.specialKeys[k] == code) kp++;
				
			} else if (this.keyCode) {
				
				if (this.keyCode == code) kp++;

			} else { //The special keys did not match
				if (character == k) {
					kp++;
				} else if(this.object.shiftNums[character] && event.shiftKey) { //Stupid Shift key bug created by using lowercase
					character = this.object.shiftNums[character]; 
					if (character == k) kp++;
				}
			}
		}

		if(kp == keys.length && 
					modifiers.ctrl.pressed == modifiers.ctrl.wanted &&
					modifiers.shift.pressed == modifiers.shift.wanted &&
					modifiers.alt.pressed == modifiers.alt.wanted &&
					modifiers.meta.pressed == modifiers.meta.wanted) {
			this.callback(event);

			if(!this.propagate) { //Stop the event
				event.stop();
				return false;
			}
		}
	},

	add: function(shortcut, callback, opt) {
		//Provide a set of default options
		var defaultOptions = {
			event:				'keydown'
			,propagate:			false
			,disableInFields:	false
			,target:			document
			,keyCode:			false
		};

		if (!opt) {
			opt = defaultOptions;
		} else {
			for(var dfo in defaultOptions) {
				if(typeof opt[dfo] == 'undefined') opt[dfo] = defaultOptions[dfo];
			}
		}
		opt.shortcut = shortcut = shortcut.toLowerCase();
		opt.object = this;
		opt.callback = callback;


		var element = opt.target;
		if (typeof opt.target == 'string') element = $(opt.target);

		var observer = this.handler.bindAsEventListener(opt);

		if (this.all[shortcut]) this.remove(shortcut);

		this.all[shortcut] = {
			element:	element
			,observer:	observer
			,event:		opt['event']
		};
		Event.observe(element, opt['event'], observer);
	},

	remove: function(shortcut) {
		shortcut = shortcut.toLowerCase();
		if (this.all[shortcut]) {
			Event.stopObserving(this.all[shortcut].element, this.all[shortcut].event, this.all[shortcut].observer);
			delete(this.all[shortcut]);
		}
	},

	removeAll: function() {
		for(var s in this.all) {
			Event.stopObserving(this.all[s].element, this.all[s].event, this.all[s].observer);
			delete(this.all[s]);
		}
		this.all = {};
	}
});