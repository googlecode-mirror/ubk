/**
 * CORE: chiamate del framework
 */
var Ubk = {

	followLink: function(l)
	{
		if ($(l)) eval($(l).href.substr(11));
	},

	// fallimento chiamata AJAX
	failure: function(request)
	{
		Ubk.working(false);
		Ubk.error(request.responseText);
	},

	// verifica successo chiamata AJAX senza chiamata in cascata (reazione)
	success: function(request, target)
	{
		Ubk.working(false);

		if ((ko = request.responseText.indexOf('+KO')) != -1) {

			if (target) Element.setStyle(target, {display: 'none'});
			Ubk.error(request.responseText.substr(ko + 3));
		
		} else if (request.responseText.indexOf('+OK') == 0) {	// ok e oks ..

			var ok_msg = request.responseText.substr(3);
			if (ok_msg.indexOf('javascript:') == 0)
				eval(ok_msg.substr(11));
			else if (ok_msg.length > 0)
				Ubk.ok(ok_msg);

		} else {
			if (target) Element.update(target, request.responseText);
			if (target && !Ubk.isVisible(target) && !$(target).hasAttribute('hide')) {
				Ubk.show(target);
			}
		}
	},

	// verifica successo chiamata AJAX con reazione in cascata
	transit: function(request, target, onComplete)
	{
		if ((ko = request.responseText.indexOf('+KO')) != -1) {

			Ubk.working(false);
			if (target) Element.setStyle(target, {display: 'none'});
			Ubk.error(request.responseText.substr(ko + 3));
		
		} else if (request.responseText.indexOf('+OK') == 0) {	// ok e oks ..

			var ok_msg = request.responseText.substr(3);
			if (ok_msg.indexOf('javascript:') == 0)
				eval(ok_msg.substr(11));
			else if (ok_msg.length > 0)
				Ubk.ok(ok_msg);

			if (onComplete) onComplete(request, target);

		} else {
			
			Ubk.working(false);
			if (target) Element.update(target, request.responseText);
			if (onComplete) onComplete(request, target);
			if (target && !Ubk.isVisible(target) && !$(target).hasAttribute('hide')) {
				Ubk.show(target);
			}
		}
	},

	getRequest: function(o) {
		new Ajax.Request(
			o.options.follow.url,
			{
				method: 'get'
				, parameters: o.options.follow.pars
				, evalScripts: o.options.follow.replace
				, onFailure: Ubk.failure
				, onComplete: (!o.options.follow.onComplete ? 
							function(request) { Ubk.success(request, o.options.follow.target); } :
							function(request) { Ubk.transit(request, o.options.follow.target, o.options.follow.onComplete); })
				, asynchronous: o.options.follow.async
			});
	},

	postRequest: function(o) {
		new Ajax.Request(
			o.options.follow.url,
			{
				method: 'post'
				,evalScripts: o.options.follow.replace
				,parameters: o.options.follow.pars + '&' + Form.serialize(document.forms[o.options.follow.form]) + '&FORM-NAME=' + o.options.follow.form
				,onFailure: Ubk.failure
				,onComplete: (!o.options.follow.onComplete ? 
							function(request) { Ubk.success(request, o.options.follow.target); } : 
							function(request) { Ubk.transit(request, o.options.follow.target, o.options.follow.onComplete); })
				,asynchronous: o.options.follow.async
			});
	},


	// post di dati
	post: function(form, id, target, url, pars, replace, async, onComplete) 
	{
		if (form.onsubmit) {
			var body = 'onsubmit = ' + form.onsubmit.toString().replace(/#id/g, id);
			eval(body);
		} else {
			var onsubmit = function() { return true; }
		}

		if (onsubmit.call()) {

			Ubk.working(true, 
				{ 
					form: form.name
					, target: target
					, url: url
					, pars: pars
					, replace: replace
					, async: async
					, type: 'post'
					, request: Ubk.postRequest
					, onComplete: onComplete
				});

			return true;
		} else {
			return false;
		}
	},

	silentPost: function(form, id, target, url, pars, replace, async, onComplete) 
	{
		Ubk.post(form, id, target, url, pars, replace, async, onComplete);
	},

	// get di dati
	follow: function(target, url, pars, replace, async, onComplete)
	{
		Ubk.working(true, 
			{ 
				target: target
				, url: url
				, pars: pars
				, replace: replace
				, async: async
				, type: 'get'
				, request: Ubk.getRequest
				, onComplete: onComplete
			});
	},

	// stiamo lavorando ....
	working: function (state, options)
	{
		opt = { duration: 0.1 }
		if (state) {
			if (options.target && options.pars.indexOf('&TARGET=') == -1)
				options.pars += '&TARGET=' + options.target;
			opt.follow = options;

			History.add(options.url+'?'+options.pars, options);

			options.request({options: opt});
/*
			if (true || options.async) {
				opt.afterFinish = options.afterFinish;
				new Effect.Appear('progress', opt);
			} else {
				new Effect.Appear('progress', opt);
				opt.afterFinish = options.afterFinish;
				opt.to = 0.35;
				new Effect.Appear('working', opt);
			}
		} else {
			if (Ubk.isVisible(target)) new Effect.Fade('progress', opt);
			new Effect.Fade('working', opt);*/
		}
	},

	// message boxes
	confirm: function (message)
	{
		return confirm(message);
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

};

// per le CHECKBOX-IMG
var Check = {

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

	keyToggle: function(event, lock)
	{
		if (lock) return;
		if (event.keyCode == 32)
			Check.checkToggle(Event.element(event), lock);
	}

};

History = {
	// se sto tornando indietro, non devo storicizzare
	backing: false,
	// mi dice se la chiamata è alla root o è un refresh
	isRoot: function()
	{
		return dhtmlHistory.getCurrentLocation() == '';
	},
	// afterfinish viene impostata in xp.xml dove avviene
	// questa chiamata: contiene la home predefinita dell'utente
	// o la pagina richiesta tramite virtu-link
	init: function(afterFinish)
	{
		dhtmlHistory.initialize();
		dhtmlHistory.addListener(History.show);
		// se questa è una chiamata alla root, non un refresh,
		// carico quanto devo caricare
		if (History.isRoot()) {
			Try.these(afterFinish);
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
		if (History.backing) return;
		// se ci sono opzioni, è una mia chiamata ...
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
		History.backing = true;
		// se ho opzioni, è una mia chiamata fatta durante una working
		// la rieseguo, tutti i parametri sono nelle opzioni
		if (options) {
			options.afterFinish = options.request = (options.type == 'get' ? Ubk.getRequest : Ubk.postRequest);
			Ubk.working(true, options);
		}
		// altrimenti sono tornato alla root, non faccio nulla
		History.backing = false;
	}
};
