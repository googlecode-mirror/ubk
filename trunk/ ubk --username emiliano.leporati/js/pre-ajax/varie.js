/**
 * Pulisce tutti i campi del form passato
 */
function pulisci_campi_form(form)
{
	var inp = form.getElementsByTagName("INPUT");
	var cbo = form.getElementsByTagName("SELECT");
	var txta = form.getElementsByTagName("TEXTAREA");

	for(var i = 0; i < inp.length; i ++) {
		if (inp[i].type.toUpperCase() == "CHECKBOX")
			inp[i].checked = false;
		else if (inp[i].type.toUpperCase() == "RADIO")
			inp[i].selected = false;
		else if (inp[i].name != "checkboxes")
			inp[i].value = '';
	}
	for(var i = 0; i < txta.length; i ++) {
		txta[i].value = '';
	}
	for(var i = 0; i < cbo.length; i ++) {
		cbo[i].selectedIndex = 0;
	}
}

/**
 * Blocca il bubbling dell'evento - cross browser
 */
function stop_event_propagation(event)
{
	if (event.stopPropagation) {
		event.stopPropagation();
	} else {
		event.cancelBubble = true;
	}
}

/**
 * Dice se elem appare fra gli elementi di a
 */
function in_array(elem, a)
{
	for(var i = 0; i < a.length; i++)
		if(a[i] == elem)
			return true;

	return false;
}

/**
 * Comanda un refresh sulla pagina che ha aperto la pop-up
 */
function aggiorna_padre()
{
	if (window.opener != null) {
		window.opener.location.reload();
	}
}

/**
 * Trova l'antenato di nome tagName dell'oggetto obj, se esiste - null altrimenti
 */
function trova_padre(obj, tagName)
{
	for(var parent = obj.parentNode; parent.tagName != tagName && parent.parentNode != null; parent = parent.parentNode);
	return (parent.tagName == tagName ? parent : null);
}

/**
 * Trova il discendente diretto di nome tagName dell'oggetto obj, se esiste - null altrimenti
 * fa una discesa secca a sinistra 
 */
function trova_figlio(obj, tagName)
{
	// prima becco il primo figlio di tipo 1
	for(var i = 0; i < obj.childNodes.length && obj.childNodes[i].nodeType != 1; i++);
	if (i == obj.childNodes.length) {
		return null;
	} else {
		var child = obj.childNodes[i];
		if (child.tagName == tagName) {
			return child;
		} else {
			return trova_figlio(child, tagName);
		}
	}
}

/**
 * Dovrebbe funzionare meglio della funzione sopra. Gira ricorsivamente.
 */
 
function trova_figlio_completo(obj, tagName)
{
	for(var i = 0; i < obj.childNodes.length; i++) {
		var child = obj.childNodes[i];
		if (child.tagName == tagName) {
			return child;
		} else {
			if ((maybe = trova_figlio_completo(child, tagName)) != null)
				return maybe;
		}
	}
	return null;
}

// Questa sotto non funziona. Non so il perchè ma è cosi. By Keller!!!!
/*
function trova_figlio_completo(obj, tagName)
{

	for(var i = 0; i < obj.childNodes.length; i++) {
		var child = obj.childNodes[i];
		if (child.tagName.toLowerCase() == tagName.toLowerCase()) {
			return child;
		} else {

			if ((maybe = trova_figlio_completo(child, tagName)) != null){
				return maybe;
			}
		}
	}
	return null;
}
*/

/**
 * Centra un elemento (con posizionamento assoluto) all'interno della pagina
 */
function centra(obj)
{
	obj.style.top = (document.body.offsetHeight - obj.offsetHeight) / 2;
	obj.style.left = (document.body.offsetWidth - obj.offsetWidth) / 2;
}

/**
 * Imposta la proprietà style.cursor di tutti i sotto-elementi di quello passato. Ricorsiva. Non ha effetto sull'elemento 'help_close', la X di chiusura dell' 'help_baloon'
 */
function imposta_cursore_elemento(obj, tipo_cursore)
{
	if (obj.id == 'help_close') return;
	if (obj.style != undefined) {
		obj.style.cursor = tipo_cursore;
	}
	for(var i=0; i < obj.childNodes.length; i++) {
		imposta_cursore_elemento(obj.childNodes[i], tipo_cursore);
	}
}

/**
 * Imposta la proprietà style.cursor di tutti i sotto-elementi di ogni elemento il cui TagName corrisponde a quello passato.
 */
function imposta_cursore_tag(tag, tipo_cursore)
{
	for(var i = 0, objs = document.getElementsByTagName(tag); i < objs.length; i++) {
		imposta_cursore_elemento(objs[i], tipo_cursore);
	}
}

/*******************************************
 *
 * R O L L O V E R   R I G H E   X T A B L E
 *
 *******************************************/

var gfx_row_classes = new Array();

function gfx_row_over(event, obj)
{
 	for(var i = 0; i < obj.childNodes.length - 0; i ++) {
		gfx_row_classes[i] = obj.childNodes[i].className;
		obj.childNodes[i].className += " over";
	}
	stop_event_propagation(event);
}

function gfx_row_out(event, obj)
{
	for(var i = 0; i < obj.childNodes.length - 0; i ++) {
		obj.childNodes[i].className = gfx_row_classes[i];
	}
	stop_event_propagation(event);
}


/************************************
 *
 * R O L L O V E R    I M M A G I N I
 *
 ************************************/

/**
 * Cambia src strippando Dis
 */
function img_over(obj)
{
	if (!obj.src) return;
	if (obj.src.indexOf('Dis.') != -1){
		var ext = obj.src.substring(obj.src.lastIndexOf('.'));
		var img = obj.src.substring(0, obj.src.lastIndexOf('.') - 3);

		obj.src = img + ext;
	}
}

/**
 * Cambia src aggiungendo Dis
 */
function img_out(obj)
{
	if (!obj.src) return;
	if (obj.src.indexOf('Dis.') == -1){
		var ext = obj.src.substring(obj.src.lastIndexOf('.'));
		var img = obj.src.substring(0, obj.src.lastIndexOf('.'));

		obj.src = img + 'Dis' + ext;
	}
}


/************************************
 *
 * I M G - C H E C K B O X
 *
 ************************************/

function checkImgToggle(chk, name, ext, lock)
{
	if (chk.checked) {
		src = name + (lock ? 'SIR' : 'SI') + ext;
	} else {
		src = name + (lock ? 'NOR' : 'NO') + ext;
	}
	chk.nextSibling.src = src;
}

function imgCheckToggle(img, lock)
{
	if (lock) return;

	chk = img.previousSibling;
	chk.checked = !chk.checked;
	if (chk.onclick)
		chk.onclick.call(chk);
}

/********************************
 *
 * G E S T I O N E   H E L P
 *
 ********************************/


/**
 * Attiva definitivamente la modalità help: nasconde il messaggio di alert e rende visibile l' 'help_baloon'
 */
function help_mode_on()
{
	document.getElementById('help_on').style.visibility = 'hidden';
	document.getElementById('help_baloon').style.visibility = 'visible';
	document.getElementById('help_box').innerHTML = '<span class="help_error">Nessun elemento selezionato</span>';
}

/**
 * Attiva e disattiva la modalità di help. All'attivazione, viene visualizzato un messaggio di alert per 1.5 secondi.
 */
function help_mode_toggle()
{
	if (help_mode)	{
		document.getElementById('help_baloon').style.visibility = 'hidden';
		imposta_cursore_elemento(document.body, 'auto');
		imposta_cursore_tag("A", 'pointer');
		imposta_cursore_tag("INPUT", 'text');
		imposta_cursore_tag("TEXTAREA", 'text');
	} else {
		var div = document.getElementById('help_on');
		div.style.visibility = 'visible';
		window.setTimeout(help_mode_on, 1500);
		imposta_cursore_elemento(document.body, 'help');
	}
	help_mode = !help_mode;
}

/**
 * Cerca la documentazione relativa ad un elemento. La cerca prima a destra, e se non la trova la cerca al livello superiore
 */
function cerca_doc(obj)
{
	if (obj == null) return null;
	// prima a destra
	for(var bros = obj.nextSibling; bros != null; bros = bros.nextSibling) {
		if (bros.className == "doc_div") return bros;
	}
	return cerca_doc(obj.parentNode);
}

/**
 * Simula il click sul link entro un form dato l'id, solo quando siamo dentro un campo
 */
function form_link_click(e, link_id)
{
	var evt = (window.event == null ? e : window.event );
	var src = (window.event == null ? e.target : window.event.srcElement);

	if (src.form != null && src.form != undefined) {
		var form_n_pos = src.form.name.lastIndexOf("_");
		var form_n     = src.form.name.substr(form_n_pos + 1);
		var link = document.getElementById(link_id + "_" + form_n);
		if (link != null && link.onclick != null) {
			if (link.onclick.call(link) != false && link.href != "#")
				document.location = link.href;
			return true;
		} else {
			return false;
		}
	}
}

/**
 * Simula il click sul link dato l'id
 * Il link da chiamare ha
 */
function link_click(e, link_id)
{
	var evt = (window.event == null ? e : window.event );
	var link = document.getElementById(link_id);
	if (link != null && link.onclick != null) {
		if (link.onclick.call(link) != false && link.href != "#")
			document.location = link.href;
		return true;
	} else {
		return true;
	}
}


/**
 * Handler dell'evento onclick. Se siamo in modalità help, viene cercata la documentazione dell'elemento su cui il click è avvenuto e viene mostrata nella help_box
 */
function show_help(e)
{
/*	if (help_mode) {
		// vado a cercare un elemento di classe doc_div
		var src = (window.event == null ? e.target : window.event.srcElement);
		var help_text = cerca_doc(src);
		var help_content = document.getElementById('help_box');
		var help_node = null;

		if (help_text == null) {
			help_node = document.createElement('span');
			help_node.setAttribute("class", "help_error");
			help_node.appendChild(document.createTextNode("Nessuna documentazione per questo elemento"));
		} else {
			help_node = document.createTextNode(help_text.innerHTML);
		}

		help_content.replaceChild(help_node, help_content.firstChild);
	}*/
}

function help_legenda()
{
	var top = screen.availHeight - 330;
	var left = screen.availWidth - 260;
	var h = window.open('help/legenda.html', 'legenda', 'toolbars=no, scrollbars=yes, top='+top+', left='+left+', width=250, height=300');
	h.focus();
	return h;
}
/**
 * Handler generico per l'evento keydown. 
 */
function handle_keydown(e)
{
	var code = (window.event == null ? e.keyCode : window.event.keyCode);
	var evt  = (window.event == null ? e : window.event);
	var handled = true;
	switch (code) {
	case 123:	// F12
		var scrl = navigator.appName.toLowerCase() == "netscape" ? "yes" : "no"
		if (evt.shiftKey) {
			help_legenda();
		} else if (evt.ctrlKey) {
			var height = screen.availHeight - 30;
			var width = screen.availWidth - 10;
			var h = window.open('help/tree.xml', 'help_completo', 'toolbars=no, scrollbars=no, resizable=yes, top=0, left=0, width='+width+', height='+height);
			h.focus();
		} else if (evt.altKey) {
			if (help_page == null) {
				alert('Nessun aiuto per questa videata.');
			} else {
				var height = screen.availHeight / 3 * 2;
				var width = screen.availWidth - 20;
				var h = window.open('help/' + help_page, 'help_pagina', 'toolbars=no, scrollbars=' + scrl + ', resizable=yes, top=0, left=0, width='+width+', height='+height);
				h.focus();
			}
//		} else if (help_inline == true) {
//			help_mode_toggle();
		}
		break;
	case 13:	// INVIO
		handled = form_link_click(e, "link_filtra");
		break;
	case 107:	// + tast num
//	case 187:	// +
		handled = form_link_click(e, "link_crea");
		break;
	case 39:	// ->
		if (evt.altKey)
			handled = link_click(e, "link_successivo");
		else
			handled = false;
		break;
	case 37:	// <-
		if (evt.altKey)
			handled = link_click(e, "link_precedente");
		else
			handled = false;
		break;
	default:
		handled = false;
//		window.status = code;
	}

	return !handled;
}


function dateISO(date)
{
	return date.split('/').reverse().join();
}

/**
 * Imposta l'handler per l'evento keydown a livello di pagina
 */
document.onkeydown = handle_keydown;

/**
 * Imposta l'handler per l'evento onclick a livello di pagina, che visualizza l'help relativo all'elemento su cui il click viene eseguito
 */
document.onclick = show_help;