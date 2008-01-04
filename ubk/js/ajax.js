/**
 * CORE: chiamate del framework
 */
var CUbk = Class.create({
	initialize: function() {

		this.historyManager = null;

		Ajax.Responders.register({
			onFailure: this.failure.bind(this)
		});

//		Ajax.isFailure = this.isFailure.bind(this);
	},

	followLink: function(l)
	{
		l = $(l);
		if (l && l.href) {
			eval(l.href.substr(11));
			return true;
		} else {
			return false;
		}
	},

	// fallimento chiamata AJAX
	failure: function(request)
	{
		this.working(false);
		this.error(request.responseText);
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

			if (target) Element.setStyle(target, {display: 'none'});
			this.error(request.responseText.substr(ko + 3));
		
		} else if (request.responseText.indexOf('+OK') == 0) {	// ok e oks ..

			var ok_msg = request.responseText.substr(3);
			if (ok_msg.indexOf('javascript:') == 0)
				eval(ok_msg.substr(11));
			else if (ok_msg.length > 0)
				this.ok(ok_msg);

		} else {

			if (target) Element.update(target, request.responseText);
			if (target && !this.isVisible(target)) {
				this.show(target);
			}
		}
	},

	// verifica successo chiamata AJAX con reazione in cascata
	transit: function(request, target, onComplete)
	{
		if ((ko = request.responseText.indexOf('+KO')) != -1) {

			this.working(false);
			if (target) Element.setStyle(target, {display: 'none'});
			this.error(request.responseText.substr(ko + 3));
		
		} else if (request.responseText.indexOf('+OK') == 0) {	// ok e oks ..

			var ok_msg = request.responseText.substr(3);
			if (ok_msg.indexOf('javascript:') == 0)
				eval(ok_msg.substr(11));
			else if (ok_msg.length > 0)
				this.ok(ok_msg);

			if (onComplete) onComplete(request, target);

		} else {
			
			this.working(false);
			if (target) Element.update(target, request.responseText);
			if (onComplete) onComplete(request, target);
			if (target && !this.isVisible(target)) {
				this.show(target);
			}
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

		var onsubmit = null;

		if (oForm.onsubmit) {
			var body = 'onsubmit = ' + oForm.onsubmit.toString().replace(/#id/g, id);
			eval(body);
		} else {
			onsubmit = function() { return true; };
		}

		if (onsubmit.call()) {

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

	// stiamo lavorando ....
	working: function (state, options)
	{
		opt = { duration: 0.1 };
		if (state) {
			if (options.target && options.pars.indexOf('&TARGET=') == -1)
				options.pars += '&TARGET=' + options.target;
			//opt.ubk = options;

			if (this.historyManager) this.historyManager.add(options.url+'?'+options.pars, options);

			options.request.call(this, options);
		}
	},

	// message boxes
	confirm: function (message, onOk, onCancel)
	{
		if (confirm(message)) {
			Try.these(onOk);
		} else {
			Try.these(onCancel);
		}
	},
	
	error: function(message)
	{
		alert(message);
	},
	
	ok: function(message)
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

	imageToggle: function (chk, lock)
	{
		var src = chk.nextSibling.src;
		var name = src.substr(0, src.lastIndexOf('.') - (lock ? 3 : 2));
		var ext = src.substr(src.lastIndexOf('.'));
		if (chk.checked) {
			src = name + (lock ? 'SIR' : 'SI') + ext;
		} else {
			src = name + (lock ? 'NOR' : 'NO') + ext;
		}
		chk.nextSibling.src = src;
	},
	
	checkToggle: function(img, lock)
	{
		if (lock) return;
	
		chk = img.previousSibling;
		chk.checked = !chk.checked;
		if (chk.onclick)
			chk.onclick.call(chk);
	},

	setChecked: function(input, state)
	{
		if (input.checked != state) {
			input.checked = state;
			input.onclick.call(input);
		}
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
		});

	},

	// mi dice se la chiamata Ã¨ alla root o Ã¨ un refresh
	isRoot: function()
	{
		return dhtmlHistory.getCurrentLocation() == '';
	},
	
	// afterfinish viene impostata in xp.xml dove avviene
	// questa chiamata: contiene la home predefinita dell'utente
	// o la pagina richiesta tramite virtu-link
	start: function(firstHistoryCall)
	{
		dhtmlHistory.addListener(this.show.bind(this));
		// se questa Ã¨ una chiamata alla root, non un refresh,
		// carico quanto devo caricare
		if (this.isRoot()) {
			Try.these(firstHistoryCall);
		}
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
		// se ci sono opzioni, Ã¨ una mia chiamata ...
		// verifico che si voglia cambiare il contenuto centrale, nel caso aggiungo
		// la location in history
		if (options) {
			location = hex_md5(location);
			//location != dhtmlHistory.getCurrentLocation() 
			if (options.replace && History.isValidTarget(options.target)) {
				dhtmlHistory.add(location, options);
			}
		// tendenzialmente la prima pagina, la root, senza opzioni
		} else {
			dhtmlHistory.add(location, options);
		}
	},
	// handler per il caricamento della location dato back, fore, refresh
	show: function(location, options)
	{
		this.backing = true;
		// se ho opzioni, Ã¨ una mia chiamata fatta durante una working
		// la rieseguo, tutti i parametri sono nelle opzioni
		if (options) {
			options.afterFinish = options.request = (options.type == 'get' ? Ubk.getRequest : Ubk.postRequest).bind(Ubk);
			Ubk.working(true, options);
		}
		// altrimenti sono tornato alla root, non faccio nulla
		this.backing = false;
	}
});