/*

# istanziare sempre
var Check = new CUbkCheck();
var Fld = new CUbkFieldValidate();
var Ubk = new CUbk();

# opzionale
var History = new CUbkHistory();


*/


/**
 * CORE: chiamate del framework
 */
var Locale = {
	string: function(id)
	{
		var s = '';
		var sa = [];
		
		if (historyStorage.hasKey('s'+id)) {
			s = historyStorage.get('s'+id);
		} else {
			var onComplete = function(request) {
				s = request.responseText;
				historyStorage.put('s'+id, s);
				if (historyStorage.hasKey('strings')) {
					sa = historyStorage.get('strings');
				}
				sa.push('s'+id);
				historyStorage.put('strings', sa);
			};
			Ubk.follow(null, 'general.php', 'AZIONE=get_lang&ID='+id, false, false, onComplete);
		}

		return s;
	},
	clear: function()
	{
		if (historyStorage.hasKey('strings')) {
			var sa = historyStorage.get('strings');
			sa.each(function(s) {
				historyStorage.remove(s);
			});
			historyStorage.remove('strings');
		}
	}
};

/**
 * CORE: chiamate del framework
 */
var CUbk = Class.create({
	initialize: function() {

		this.historyManager = null;

		Ajax.Responders.register({
			onFailure: this.failure.bind(this)
			,onException: this.exception.bind(this)
		});

		Event.observe(document, 'dom:loaded', this.setup.bindAsEventListener(this));
	},

	setup: function() {},
 
	tryThis: function(f) {

		if (f == undefined || f == null) return; //f = Prototype.K.curry(0);
		Try.these(f);
	},

	followLink: function(l)
	{
		l = $(l);
		if (l && l.href) {
			eval(decodeURIComponent(l.href).substr(11));
			return true;
		} else {
			return false;
		}
	},

	isOpenPage: function(event)
	{
		event = Element.extend(event);
		return ((event.isLeftClick() && event.ctrlKey) || event.isMiddleClick());
	},

	openPage: function(event, url)
	{
		if (this.isOpenPage(event)) {
			window.open(url, '', '');
			window.focus();
			return true;
		} else {
			return false;
		}
	},
	// fallimento ed eccezione chiamata AJAX
	failure: function(request)
	{
		this.working(false);
		this.error(request.responseText);
	},
	exception: function(request, e)
	{
		this.working(false);
		this.alert('<pre style="font-size: 7pt">'+[e.name, e.message].join(': ') + "\n" + e.stack + '</pre>');
	},

	isFailure: function(request)
	{
		var ko = request.responseText.indexOf('+KO');
		if (ko != -1) {
			this.error(request.responseText.substr(ko + 3));
			return true;
		} else {
			return false;
		}
	},

	// verifica successo chiamata AJAX senza chiamata in cascata (reazione)
	success: function(request, target)
	{
		this.working(false);

		if ((ko = request.responseText.indexOf('+KO')) != -1) {

			if (target && $(target)) this.hide(target);
			this.error(request.responseText.substr(ko + 3));
		
		} else if (request.responseText.indexOf('+OK') == 0) {	// ok e oks ..

			var ok_msg = request.responseText.substr(3);
			if (ok_msg.indexOf('javascript:') == 0)
				eval(ok_msg.substr(11));
			else if (ok_msg.length > 0)
				this.alert(ok_msg);

		} else {

			if (target && $(target)) Element.update(target, request.responseText);
			if (target && $(target)) this.show(target);
		}
	},

	// verifica successo chiamata AJAX con reazione in cascata
	transit: function(request, target, onComplete)
	{
		if ((ko = request.responseText.indexOf('+KO')) != -1) {

			this.working(false);
			if (target && $(target)) this.hide(target);
			this.error(request.responseText.substr(ko + 3));
		
		} else if (request.responseText.indexOf('+OK') == 0) {	// ok e oks ..

			var ok_msg = request.responseText.substr(3);
			if (ok_msg.indexOf('javascript:') == 0)
				eval(ok_msg.substr(11));
			else if (ok_msg.length > 0)
				this.alert(ok_msg);

			if (onComplete) onComplete(request, target);

		} else {
			
			this.working(false);
			if (target && $(target)) Element.update(target, request.responseText);
			if (onComplete) onComplete(request, target);
			if (target && $(target)) this.show(target);
		}
	},

	getRequest: function(opt) {
		var onComplete = (!opt.onComplete ? 
					function(request) { this.success(request, opt.target); } :
					function(request) { this.transit(request, opt.target, opt.onComplete); });
		new Ajax.Request(
			opt.url,
			{
				method: 'get'
				, parameters: opt.pars
				, evalScripts: opt.replace
				, onFailure: this.failure.bind(this)
				, onComplete: onComplete.bind(this)
				, asynchronous: opt.async
			});
	},

	postRequest: function(opt) {
		var onComplete = (!opt.onComplete ? 
					function(request) { this.success(request, opt.target); } : 
					function(request) { this.transit(request, opt.target, opt.onComplete); });
		new Ajax.Request(
			opt.url,
			{
				method: 'post'
				,evalScripts: opt.replace
				,parameters: opt.pars + '&' + $(document.forms[opt.form]).serialize() + '&FORM-NAME=' + opt.form
				,onFailure: this.failure.bind(this)
				,onComplete: onComplete.bind(this)
				,asynchronous: opt.async
			});
	},


	// post di dati
	post: function(form, id, target, url, pars, replace, async, onComplete) 
	{
		var oForm = document.forms[form];

		var sbmit = null;

		if (oForm.onsubmit) {
			sbmit = function() {
				var o = { ROWID: id };
				var res;
// 				eval(oForm.onsubmit.toString().replace(/#id/g, id));
				try {
					res = oForm.onsubmit.call(o);
				} catch (e) {
					this.exception(null, e);
					res = false;
				}
				return res;
			};
		} else {
			sbmit = function() { return true; };
		}

		if (sbmit.call()) {

			this.working(true, 
				{ 
					form: form
					, target: target
					, url: url
					, pars: pars
					, replace: replace
					, async: async
					, type: 'post'
					, request: this.postRequest
					, onComplete: onComplete
				});

			return true;
		} else {
			return false;
		}
	},

	silentPost: function(form, id, target, url, pars, replace, async, onComplete) 
	{
		this.post(form, id, target, url, pars, replace, async, onComplete);
	},

	// get di dati
	follow: function(target, url, pars, replace, async, onComplete)
	{
		this.working(true, 
			{ 
				target: target
				, url: url
				, pars: pars
				, replace: replace
				, async: async
				, type: 'get'
				, request: this.getRequest
				, onComplete: onComplete
			});
	},

	workingState: function(state)
	{},

	// stiamo lavorando ....
	working: function (state, options)
	{
		this.workingState(state);

		if (state) {

			if (options.target && options.pars.indexOf('&TARGET=') == -1)
				options.pars += '&TARGET=' + options.target;

			if (options.type == 'get' && this.historyManager) this.historyManager.add(options.url+'?'+options.pars, options);

			options.request.call(this, options);
		}
	},

	// message boxes
	confirm: function (message, onOk, onCancel)
	{
		if (confirm(message)) {
			this.tryThis(onOk);
		} else {
			this.tryThis(onCancel);
		}
	},
	
	error: function(message, onOk)
	{
		alert(message);
		this.tryThis(onOk);
	},
	
	alert: function(message)
	{
		alert(message);
	},

	isVisible: function(target)
	{
		return false;
	},

	show: function(target)
	{
		Element.show(target);		
	},

	hide: function(target)
	{
		Element.hide(target);
	}

});

// per le CHECKBOX-IMG
var CUbkCheck = Class.create({

	imageToggle: function (input, lock)
	{
		var src = input.nextSibling.src;
		var name = src.substr(0, src.lastIndexOf('.') - (lock ? 3 : 2));
		var ext = src.substr(src.lastIndexOf('.'));
		if (input.checked) {
			src = name + (lock ? 'SIR' : 'SI') + ext;
		} else {
			src = name + (lock ? 'NOR' : 'NO') + ext;
		}
		input.nextSibling.src = src;
	},
	
	checkToggle: function(img, lock)
	{
		if (lock) return;
	
		var input = img.previousSibling;
		input.checked = !input.checked;
		if (input.onclick)
			input.onclick.call(input);
	},

	setChecked: function(input, state)
	{
		if (input.checked != state) {
			input.checked = state;
		}
		input.onclick.call(input);
	},

	// toggle su barra spaziatrice
	keyToggle: function(event, lock)
	{
		if (lock) return;
		if (event.keyCode == 32)
			this.checkToggle(Event.element(event), lock);
	}

});

var CUbkHistory = Class.create({
	initialize: function() {
		// se sto tornando indietro, non devo storicizzare
		this.backing = false;
		this.started = false;
		// link con Ubk
		Ubk.historyManager = this;
		// init dhtmlHistory
		dhtmlHistory.create({
			toJSON: function(o) {
					return Object.toJSON(o);
			}
			, fromJSON: function(s) {
					return s.evalJSON();
			}
			, baseTitle: $$('head title').reduce().textContent + ' - @@@'
		});

		Event.observe(document, 'dom:loaded', (function() {
			dhtmlHistory.initialize(this.show.bind(this));
// 			dhtmlHistory.addListener(this.show.bind(this));
		}).bind(this));
	},

	// mi dice se la chiamata e' alla root o e' un refresh
	isRoot: function()
	{
		return dhtmlHistory.getCurrentLocation() == '';
	},
	
	// afterfinish viene impostata in xp.xml dove avviene
	// questa chiamata: contiene la home predefinita dell'utente
	// o la pagina richiesta tramite virtu-link
	start: function(firstHistoryCall)
	{
		if (this.started) return;
		// se questa e' una chiamata alla root, non un refresh,
		// carico quanto devo caricare
		if (this.isRoot()) {
			Ubk.tryThis(firstHistoryCall);
		} else {
			if (!Prototype.Browser.Gecko) Ubk.tryThis(firstHistoryCall);
		}
		this.started = true;
	},
	
	// mi dice se il target object puo' essere considerato per
	// mantenere la history
	isValidTarget: function(target)
	{
		return true;
	},
	// questo viene chiamato da working, quando vengono fatte le chiamate ajax
	add: function(location, options)
	{
		// se sono arrivato qua mentre tornavo indietro, non aggiorno la history
		if (this.backing) return;
		// se ci sono opzioni, e' una mia chiamata ...
		// verifico che si voglia cambiare un contenuto storicizzabile
		// nel caso aggiungo la location in history
		if (options) {
			location = hex_md5(location);
			if (options.replace && History.isValidTarget(options.target)) {
				dhtmlHistory.add(location, options);
			}
		// tendenzialmente la prima pagina, la root, senza opzioni
		} else {
			dhtmlHistory.add(location, options);
		}
	},
	defaultShow: function()
	{
	},
	// handler per il caricamento della location dato back, fore, refresh
	show: function(location, options)
	{
		this.backing = true;
		// se ho opzioni, e' una mia chiamata fatta durante una working
		// la rieseguo, tutti i parametri sono nelle opzioni
		if (options) {
			if (options.type == 'get') {
				options.afterFinish = options.request = Ubk.getRequest.bind(Ubk);
				Ubk.working(true, options);
			// i post li salto
			} else {
				history.back();
			}
		} else {
			this.defaultShow();
		}
		// altrimenti sono tornato alla root, non faccio nulla
		this.backing = false;
	}
});