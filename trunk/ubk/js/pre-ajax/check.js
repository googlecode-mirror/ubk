/**
 * Dice se il campo (input o textarea) � vuoto
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function campo_vuoto(campo)
{
	if (false == (obj = getEl(campo)))
		return true;
	else
		return (obj.value.length == 0);
}

/**
 * Dice se il campo (input o textarea) � vuoto + alert + focus
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function campo_obb_vuoto(campo)
{
	if (false == (obj = getEl(campo)))
		return true;
	else if (obj.value.length == 0) {
		alert("Manca un valore necessario !");
		ensureVisible(obj);
		return true;
	} else {
		return false;
	}
}


/**
 * Dice se la combo non ha alcun elemento selezionato
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function combo_vuota(campo, id)
{
	if (false == (obj = getEl(campo)))
		return true;
	else
		return (obj.value.length == 0);
}

/**
 * Dice se la combo non ha alcun elemento selezionato + alert + focus
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function combo_obb_vuota(campo)
{
	if (false == (obj = getEl(campo)))
		return true;
	else if (obj.value.length == 0) {
		alert("Manca un valore necessario !");
		ensureVisible(obj);
		return true;
	} else {
		return false;
	}
}

/**
 * Dice se il picker non ha alcun elemento selezionato + alert + focus
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function pick_obb_vuoto(campo)
{
	if (false == (obj = getEl(campo)))
		return true;
	else if (obj.value.length == 0) {
		alert("Manca un valore necessario !");
		ensureVisible(obj.nextSibling);
		return true;
	} else {
		return false;
	}
}

/**
 * Dice se il radio group non ha alcun elemento selezionato
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function radio_vuota(campo)
{
	if (false == (obj = getEl(campo)))
		return true;

	for(var i = 0; i < obj.length; i++) {
		if (obj[i].checked) return false;
	}
	return true;
}

/**
 * Dice se il radio group non ha alcun elemento selezionato + alert + focus
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function radio_obb_vuoto(campo)
{
	if (false == (obj = getRadio(campo)))
		return true;

	for(var i = 0; i < obj.length; i++) {
		if (obj[i].checked) return false;
	}

	alert("Manca un valore necessario !");
	ensureVisible(obj[0]);
	return true;
}


/**
 * Ritorna il campo con l'id specificato o false + alert se non esiste (errore di coding)
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function getEl(id)
{
	var obj = document.getElementById(id);
	if (obj == null) {
		alert("Attenzione coder, campo '" + id + "' non trovato !");
		return false;
	} else {
		return obj;
	}
}


function getField(name, id)
{
	return getEl(name + '[' + id + ']');
}

/**
 * Ritorna i radio con il nome specificato o false + alert se non esiste (errore di coding)
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function getRadio(name)
{
	var obj = document.getElementsByName(name);
	if (obj == null) {
		alert("Attenzione coder, campo '" + id + "' non trovato !");
		return false;
	} else {
		return obj;
	}
}

/**
 * Sposta il fuoco su un oggetto, assicurandosi che esso sia visibile - quindi se il campo si trova in una tabstrip, attiva la scheda in cui il campo si trova
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function ensureVisible(obj)
{
	// vediamo se risiede dentro un tabstrip_content
	for(var p = obj.parentNode; p != null; p = p.parentNode) {
		if (p.id != null && p.id.substr(0, 16) == 'tabstripcontent_') break;
	}

	if (p != null) {
		var a = p.id.split('_');
		var hd = document.getElementById('tabstriphead_' + a[1] + '_' + a[2]);
		tabstrip_roll(hd);
	}
	if (!obj.disabled) obj.focus();
}

/**
 * Verifica che in una casella di testo sia presente un indirizzo di posta elettronica.
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_mail(obj)
{
	if (obj.value.length == 0) return true;

	var is_mail = new RegExp("^[\\w\\-\\.]*[\\w\\.]\\@[\\w\\.]*[\\w\\-\\.]+[\\w\\-]+[\\w]\\.+[\\w]+$");
	return is_mail.test(obj.value);
}

/**
 * Verifica che in una casella di testo sia presente un codice fiscale (non necessariamente valido)
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_cf(obj)
{
	if (obj.value.length == 0) return true;

	var is_cf = new RegExp("^[a-zA-Z]{6}[0-9]{2}[a-zA-Z][0-9]{2}[a-zA-Z][0-9]{3}[a-zA-Z]$");
	return is_cf.test(obj.value);
}

/**
 * Verifica che in una casella di testo sia presente un indirizzo web.
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_url(obj)
{
	if (obj.value.length == 0) return true;

	var is_url = new RegExp('^(([\\w\\-\\.]+)@)?((\\[([0-9]{1,3}\.){3}[0-9]{1,3}\\])|(([\\w\\-]+\\.)+)([a-zA-Z]{2,4}))$');
	return is_url.test(obj.value);
}

/**
 * Verifica che in una casella di testo sia presente un numero decimale. In realt� questa funzione toglie tutti i caratteri non numerici e tutti i punti di separazione decimale (. o ,) successivi al primo, quindi alla fine dell'esecuzione il contenuto sar� sempre un numero decimale - il check ha sempre successo.
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_float(obj)
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
}

/**
 * Verifica che in una casella di testo sia presente un numero intero. In realt� questa funzione toglie tutti i caratteri non numerici, quindi alla fine dell'esecuzione il contenuto sar� sempre un numero intero - il check ha sempre successo.
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_int(obj)
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
}

function check_year(obj) 
{
	var is_year = new RegExp("^[1-9][0-9][0-9][0-9]$");
	return is_year.test(obj.value);
}

/**
 * Verifica che in una casella di testo sia presente un "time" (hh.mm). Questa funzione effettua le seguenti trasformazioni: hh -> hh.00; hhmm -> hh.mm; h -> 0h.00; hmm -> 0h.mm
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_hour(obj)
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
		alert("Formato ora errato !");
		obj.value = '';
	}
	return res;
}


/**
 * Verifica che in una casella di testo sia presente un "date" (dd/mm/yyyy). Questa funzione effettua le seguenti trasformazioni: d -> 0d; m -> 0m; yy -> 20yy; yyy -> 2yyy; e tutte le combinazioni (es: d/m/yy -> 0d/0m/20yy). Verifica anche il 29 febbraio sugli anni bisestili
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function check_data(obj, century)
{
	var d = obj.value;

	if (d.length == 0) return true;

	var i1 = d.indexOf('/');
	if (i1 == -1) {
		g = d;
		if (g < 0 || g > 31) {
			alert("Giorno sbagliato !");
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
			alert("Giorno sbagliato !");
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
				alert("Giorno sbagliato !");
				obj.focus();
				return false;
			}
			if (m >= "01" && m <= "12") break;
		default:
			alert("Mese sbagliato !");
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
				alert("Giorno sbagliato !");
				obj.focus();
				return false;
			}
		}
		break;
	default:
		alert("Anno sbagliato !");
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
		alert("Anno oltre i limiti ! Intervallo valido: 1900 - 2050");
		obj.focus();
		return false;
	}

	obj.value = d;
	return true;
}
