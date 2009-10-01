var CUbkFieldCheck = Class.create({
	suggested: function(fields, messages)
	{
		var msg = new Array(); 
		var i = 0; 
		var acapo = String.fromCharCode(10, 13);

		fields.each(function(f, idx) {
			if ($(f) && $F(f).length == 0) msg[i++] = messages[idx];
		});

		if (msg.length > 0) {
			return confirm(Locale.string('CHK-SUGGESTED-PRE') 
				+ acapo + acapo + msg.join(', ' + acapo) 
				+ acapo + acapo + Locale.string('CHK-SUGGESTED-POST'));
		} else {
			return true;
		}
	},

	mandatory: function(fields)
	{
		
		for(i = 0; i < fields.length; i++) {
			if ($(fields[i]) && $F(fields[i]).length == 0) {
				Ubk.error(Locale.string('CHK-MISSING'));
				Form.Element.focus(fields[i]);
				return false;
			}
		}

		return true;
	},

	/**
	 * Verifica che in una casella di testo sia presente un indirizzo di posta elettronica.
	 */
	mail: function(obj)
	{
		obj.value.replace(' ', ''); 
		if (obj.value.length == 0) return true;

		var is_mail = new RegExp("^[\\w\\-\\.]*[\\w\\.]\\@[\\w\\.]*[\\w\\-\\.]+[\\w\\-]+[\\w]\\.+[\\w]+$");
		if (!is_mail.test(obj.value)) {
			Ubk.error(Locale.string('CHK-EMAIL'));
			obj.focus(); 
			return false;
		} else {
			return true;
		}
	},

	/**
	 * Verifica che in una casella di testo sia presente un codice fiscale (non necessariamente valido)
	 */
	cf: function(obj)
	{
		if (obj.value.length == 0) return true;

		var is_cf = new RegExp("^[a-zA-Z]{6}[0-9]{2}[a-zA-Z][0-9]{2}[a-zA-Z][0-9]{3}[a-zA-Z]$");
		if (!is_cf.test(obj.value)) {
			obj.focus(); 
			Ubk.error(Locale.string('CHK-CF'));
			return false;
		} else {
			return true;
		}
	},

	/**
	 * Verifica che in una casella di testo sia presente un indirizzo web.
	 */
	url: function(obj)
	{
		if (obj.value.length == 0) return true;

		var is_url = new RegExp("^(([a-z0-9]+://)(([\w\-]+\.)+\w{2,4}(:[0-9]+)?(/[^\s\"\n\]\[<]*)?))$");
		if (!is_url.test(obj.value)) {
			obj.focus(); 
			Ubk.error(Locale.string('CHK-URL'));
			return false;
		} else {
			return true;
		}
	},

	/**
	 * Verifica che in una casella di testo sia presente un numero decimale. In realta' questa funzione toglie tutti i caratteri non numerici e tutti i punti di separazione decimale (. o ,) successivi al primo, quindi alla fine dell'esecuzione il contenuto sara' sempre un numero decimale - il check ha sempre successo.
	 */
	real: function(obj)
	{
		var is_num = new RegExp("[0-9]");
		var dot_found = false;
		var result = "";
		obj.value = obj.value.replace(" ", "");
		for(var i = 0; i < obj.value.length; i++) {
			var chr = obj.value.substr(i,1)
			if (is_num.test(chr) || (i == 0 && (chr == "-" || chr == "+"))) {
				result += chr;
			} else if ((chr == "." || chr == ",") && !dot_found) {
				result += ".";
				dot_found = true;
			}
		}
		obj.value = result;
		return true;
	},

	/**
	 * Verifica che in una casella di testo sia presente un numero intero. In realta' questa funzione toglie tutti i caratteri non numerici, quindi alla fine dell'esecuzione il contenuto sara' sempre un numero intero - il check ha sempre successo.
	 */
	integer: function(obj)
	{
		var is_num = new RegExp("[0-9]");
		var result = "";
		obj.value = obj.value.replace(" ", "");
		for(var i = 0; i < obj.value.length; i++) {
			var chr = obj.value.substr(i,1)
			if (is_num.test(chr) || (i == 0 && (chr == "-" || chr == "+"))) {
				result += chr;
			} 
		}
		obj.value = result;
		return true;
	},

	year: function(obj) 
	{
		var is_year = new RegExp("^[1-9][0-9][0-9][0-9]$");
		if (!is_year.test(obj.value)) {
			Ubk.error(Locale.string('CHK-YEAR'));
			obj.focus(); 
			return false;
		} else {
			return true;
		}
	},

	/**
	 * Verifica che in una casella di testo sia presente un "time" (hh.mm). Questa funzione effettua le seguenti trasformazioni: hh -> hh.00; hhmm -> hh.mm; h -> 0h.00; hmm -> 0h.mm
	 */
	hour: function(obj)
	{
		var h = obj.value;
		if (h.length == 0) return true;

		var i1 = h.indexOf(".");
		if (i1 == -1)
			i1 = h.indexOf(":");

		if (i1 == -1) {
			if (h.length == 4) {
				obj.value = h.substring(0,2) + ":" + h.substring(2,4);
			} else if (h.length == 1) {
				obj.value = "0" + h + ":00";
			} else if (h.length == 2) {
				obj.value = h + ":00";
			}
		} else {
			hh = h.substring(0, i1);
			mm = h.substring(i1 + 1, h.length);
			if (hh.length == 1) hh = "0" + hh;
			obj.value = hh + ":" + mm;
		}
		var is_time = new RegExp("^([0-1][0-9]|2[0-3])\:[0-5][0-9]$");
		var res = is_time.test(obj.value);
		if (!res) {
			Ubk.error(Locale.string('CHK-HOUR'));
			obj.value = '';
			obj.focus();
		}
		return res;
	},


	/**
	 * Verifica che in una casella di testo sia presente un "date" (dd/mm/yyyy). Questa funzione effettua le seguenti trasformazioni: d -> 0d; m -> 0m; yy -> 20yy; yyy -> 2yyy; e tutte le combinazioni (es: d/m/yy -> 0d/0m/20yy). Verifica anche il 29 febbraio sugli anni bisestili
	 */
	dateDMY: function(obj, century)
	{
		var d = obj.value;

		if (d.length == 0) return true;

		var i1 = d.indexOf('/');
		// se non ho sbarre, ho solo il giorno
		if (i1 == -1) {
			g = d;
			if (g < 0 || g > 31) {
				Ubk.error(Locale.string('CHK-DAY'));
				obj.focus();
				return false;
			}
			if (d.length == 1) d = "0" + d;
		// almeno giorno e mese
		} else {
			switch(d.substring(0, i1).length) {
			case 1:
				d = "0" + d;
				i1 ++;
			case 2:
				var g = d.substring(0, i1);
				if (g > 0 && g <= 31) break;
			default:
				Ubk.error(Locale.string('CHK-DAY'));
				obj.focus();
				return false;
			}
			i1 ++;
		}
		// qua il giorno e' a posto

		// se ho solo il giorno, completo tutto
		if (i1 == -1) {
			var m = new String(1 + (new Date()).getMonth());
			if (m.length == 1)
				d = d + '/0' + m;
			else
				d = d + '/' + m;

			i2 = d.length;
		// vado a vedere il mese
		} else {
			var i2 = d.indexOf('/', i1);
			if (i2 == -1) i2 = d.length;

			switch(d.substring(i1,i2).length) {
			case 1:
				d = d.substring(0, i1) + '0' + d.substr(i1);
				i2 ++;
			case 2:
				var m = d.substring(i1,i2);
				// trenta giorni conta novembre, con april giugno e settembre
				if ((m == "11" || m == "04" || m == "06" || m =="09") && g > "30") {
					Ubk.error(Locale.string('CHK-DAY'));
					obj.focus();
					return false;
				}
				if (m >= "01" && m <= "12") break;
			default:
				Ubk.error(Locale.string('CHK-MONTH'));
				obj.focus();
				return false;
			}
			i2 ++;
		}

		// anno
		switch(d.substr(i2).length) {
		case 0:
			d = d + '/' + (new Date()).getFullYear();
			break;
		case 3: 
			d = d.substring(0, i2) + century.substring(0, 1) + d.substr(i2); 
			break;
		case 2:
			d = d.substring(0, i2) + century.substring(0, 2) + d.substr(i2); 
			break;
		case 4:
			var a = parseInt(d.substr(i2));
			if (m == "02") {
				if (a % 4 == 0) {
					if (a % 100 == 0) {
						max_febbraio = (a % 400 == 0) ? 29 : 28;
					} else {
						max_febbraio = 29;
					}
				} else {
					max_febbraio = 28;
				}
				if (parseInt(g) > max_febbraio) {
					Ubk.error(Locale.string('CHK-DAY'));
					obj.focus();
					return false;
				}
			}
			break;
		default:
			Ubk.error(Locale.string('CHK-YEAR'));
			obj.focus();
			return false;
		}

		var y = parseInt(d.substring(6));
		if (isNaN(y)){
			Ubk.error(Locale.string('CHK-YEAR'));
			obj.focus();
			return false;
		}
		obj.value = d;
		
		var is_date = new RegExp("^[0-9]{2}/[0-9]{2}/[0-9]{4}$");
		if (!is_date.test(obj.value)) {
			Ubk.error(Locale.string('CHK-DAY'));
			obj.value = '';
			obj.focus();
			return false;
		}

		return true;
	},

	dateMDY: function(obj, century)
	{
		var d = obj.value;

		if (d.length == 0) return true;

		var i1 = d.indexOf('/');
		// da solo, e' il giorno
		if (i1 == -1) {
			g = d;
			if (g < "0" || g > "31") {
				Ubk.error(Locale.string('CHK-DAY'));
				obj.focus();
				return false;
			}
			if (d.length == 1) d = "0" + d;
		// il primo e' il mese
		} else {
			switch(d.substring(0, i1).length) {
			case 1:
				d = "0" + d;
				i1 ++;
			case 2:
				var m = d.substring(0, i1);
				if (m > "0" && m <= "12") break;
			default:
				Ubk.error(Locale.string('CHK-MONTH'));
				obj.focus();
				return false;
			}
			i1 ++;
		}

		// se era da solo, metto il mese
		if (i1 == -1) {
			var m = new String(1 + (new Date()).getMonth());
			if (m.length == 1)
				d = '0' + m + '/' + d
			else
				d = m + '/' + d;

			i2 = d.length;
		// avevo anche il mese
		} else {
			var i2 = d.indexOf('/', i1);
			if (i2 == -1) i2 = d.length;

			switch(d.substring(i1,i2).length) {
			case 1:
				d = d.substring(0, i1) + '0' + d.substr(i1);
				i2 ++;
			case 2:
				var g = d.substring(i1,i2);
				// trenta giorni conta novembre, con april giugno e settembre
				if ((m == "11" || m == "04" || m == "06" || m =="09") && g > "30") {
					Ubk.error(Locale.string('CHK-DAY'));
					obj.focus();
					return false;
				}
				if (m >= "01" && m <= "12") break;
			default:
				Ubk.error(Locale.string('CHK-DAY'));
				obj.focus();
				return false;
			}
			i2 ++;
		}

		switch(d.substr(i2).length) {
		case 0:
			d = d + '/' + (new Date()).getFullYear();
			break;
		case 3: 
			d = d.substring(0, i2) + century.substring(0, 1) + d.substr(i2); 
			break;
		case 2:
			d = d.substring(0, i2) + century.substring(0, 2) + d.substr(i2); 
			break;
		case 4:
			var a = parseInt(d.substr(i2));
			if (m == "02") {
				if (a % 4 == 0) {
					if (a % 100 == 0) {
						max_febbraio = (a % 400 == 0) ? 29 : 28;
					} else {
						max_febbraio = 29;
					}
				} else {
					max_febbraio = 28;
				}
				if (parseInt(g) > max_febbraio) {
					Ubk.error(Locale.string('CHK-DAY'));
					obj.focus();
					return false;
				}
			}
			break;
		default:
			Ubk.error(Locale.string('CHK-YEAR'));
			obj.focus();
			return false;
		}

		var y = parseInt(d.substring(6));
		if (isNaN(y)){
			Ubk.error(Locale.string('CHK-YEAR'));
			obj.focus();
			return false;
		}
		obj.value = d;
	
		var is_date = new RegExp("^[0-9]{2}/[0-9]{2}/[0-9]{4}$");
		if (!is_date.test(obj.value)) {
			Ubk.error(Locale.string('CHK-DAY'));
			obj.value = '';
			obj.focus();
			return false;
		}

		return true;
	}
});

var CUbkFieldValidate = Class.create({
	initialize: function() {
		this.Check = new CUbkFieldCheck();
	}
});