var Fld = {
	Check: {

		suggested: function(fields, messages)
		{
			var msg = new Array(); 
			var i = 0; 
			var acapo = String.fromCharCode(10, 13);

			$A(fields).each(function(f, idx) {
				if ($(f) && $F(f).length == 0) msg[i++] = messages[idx];
			});

			if (msg.length > 0)
				return Ubk.confirm('Attenzione, si consiglia di riempire anche i seguenti campi: ' 
					+ acapo + acapo + msg.join(', ' + acapo) 
					+ acapo + acapo + 'proseguire comunque ?');
			else
				return true;
		},

		mandatory: function(fields)
		{
			for(i = 0; i < fields.length; i++) {
				if ($(fields[i]) && $F(fields[i]).length == 0) {
					Ubk.error("Manca un valore necessario !");
					Form.Element.focus(fields[i]);
					return false;
				}
			}

			return true;
		},

		/**
		 * Verifica che in una casella di testo sia presente un indirizzo di posta elettronica.
		 * @author Ubik <leporati@immaginazioneelavoro.it>
		 */
		mail: function(obj)
		{
			obj.value.replace(' ', ''); 
			if (obj.value.length == 0) return true;

			var is_mail = new RegExp("^[\\w\\-\\.]*[\\w\\.]\\@[\\w\\.]*[\\w\\-\\.]+[\\w\\-]+[\\w]\\.+[\\w]+$");
			if (!is_mail.test(obj.value)) {
				alert('Indirizzo e-mail non valido.'); 
				obj.focus(); 
				return false;
			} else {
				return true;
			}
		},

		/**
		 * Verifica che in una casella di testo sia presente un codice fiscale (non necessariamente valido)
		 * @author Ubik <leporati@immaginazioneelavoro.it>
		 */
		cf: function(obj)
		{
			if (obj.value.length == 0) return true;

			var is_cf = new RegExp("^[a-zA-Z]{6}[0-9]{2}[a-zA-Z][0-9]{2}[a-zA-Z][0-9]{3}[a-zA-Z]$");
			if (!is_cf.test(obj.value)) {
				obj.focus(); 
				Ubk.error('Codice Fiscale non valido.');
				return false;
			} else {
				return true;
			}
		},

		/**
		 * Verifica che in una casella di testo sia presente un indirizzo web.
		 * @author Ubik <leporati@immaginazioneelavoro.it>
		 */
		url: function(obj)
		{
			if (obj.value.length == 0) return true;

			var is_url = new RegExp('^(([\\w\\-\\.]+)@)?((\\[([0-9]{1,3}\.){3}[0-9]{1,3}\\])|(([\\w\\-]+\\.)+)([a-zA-Z]{2,4}))$');
			if (!is_url.test(obj.value)) {
				obj.focus(); 
				Ubk.error('URL mal formato.');
				return false;
			} else {
				return true;
			}
		},

		/**
		 * Verifica che in una casella di testo sia presente un numero decimale. In realt� questa funzione toglie tutti i caratteri non numerici e tutti i punti di separazione decimale (. o ,) successivi al primo, quindi alla fine dell'esecuzione il contenuto sar� sempre un numero decimale - il check ha sempre successo.
		 * @author Ubik <leporati@immaginazioneelavoro.it>
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
		 * Verifica che in una casella di testo sia presente un numero intero. In realt� questa funzione toglie tutti i caratteri non numerici, quindi alla fine dell'esecuzione il contenuto sar� sempre un numero intero - il check ha sempre successo.
		 * @author Ubik <leporati@immaginazioneelavoro.it>
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
				Ubk.error('Anno non valido.');
				obj.focus(); 
				return false;
			} else {
				return true;
			}
		},

		/**
		 * Verifica che in una casella di testo sia presente un "time" (hh.mm). Questa funzione effettua le seguenti trasformazioni: hh -> hh.00; hhmm -> hh.mm; h -> 0h.00; hmm -> 0h.mm
		 * @author Ubik <leporati@immaginazioneelavoro.it>
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
					obj.value = h.substring(0,2) + "." + h.substring(2,4);
				} else if (h.length == 1) {
					obj.value = "0" + h + ".00";
				} else if (h.length == 2) {
					obj.value = h + ".00";
				}
			} else {
				hh = h.substring(0, i1);
				mm = h.substring(i1 + 1, h.length);
				if (hh.length == 1) hh = "0" + hh;
				obj.value = hh + "." + mm;
			}
			var is_time = new RegExp("^([0-1][0-9]|[2][0-3])\.[0-5][0-9]$");
			var res = is_time.test(obj.value);
			if (!res) {
				alert("Formato ora errato.");
				obj.value = '';
				obj.focus();
			}
			return res;
		},


		/**
		 * Verifica che in una casella di testo sia presente un "date" (dd/mm/yyyy). Questa funzione effettua le seguenti trasformazioni: d -> 0d; m -> 0m; yy -> 20yy; yyy -> 2yyy; e tutte le combinazioni (es: d/m/yy -> 0d/0m/20yy). Verifica anche il 29 febbraio sugli anni bisestili
		 * @author Ubik <leporati@immaginazioneelavoro.it>
		 */
		date: function(obj, century)
		{
			var d = obj.value;

			if (d.length == 0) return true;

			var i1 = d.indexOf('/');
			if (i1 == -1) {
				g = d;
				if (g < 0 || g > 31) {
					alert("Giorno sbagliato.");
					obj.focus();
					return false;
				}
				if (d.length == 1) d = "0" + d;
			} else {
				switch(d.substring(0, i1).length) {
				case 1:
					d = "0" + d;
					i1 ++;
				case 2:
					var g = d.substring(0, i1);
					if (g > 0 && g <= 31) break;
				default:
					alert("Giorno sbagliato.");
					obj.focus();
					return false;
				}
				i1 ++;
			}

			if (i1 == -1) {
				var m = new String(1 + (new Date()).getMonth());
				if (m.length == 1)
					d = d + '/0' + m;
				else
					d = d + '/' + m;

				i2 = d.length;
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
					if ((m == "11" || m == "04" || m == "06" || m =="09") && g > 30) {
						alert("Giorno sbagliato.");
						obj.focus();
						return false;
					}
					if (m >= "01" && m <= "12") break;
				default:
					alert("Mese sbagliato.");
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
				var a = d.substr(i2);
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
					if (g > max_febbraio) {
						alert("Giorno sbagliato.");
						obj.focus();
						return false;
					}
				}
				break;
			default:
				alert("Anno sbagliato.");
				obj.focus();
				return false;
			}

			var y = parseInt(d.substring(6));
			if (isNaN(y)){
				alert("L'anno non e' numerico");
				obj.focus();
				return false;
			}

			if (y < 1900 || y > 2050) {
				alert("Anno oltre i limiti. Intervallo valido: 1900 - 2050");
				obj.focus();
				return false;
			}

			obj.value = d;
			return true;
		}
	}
}