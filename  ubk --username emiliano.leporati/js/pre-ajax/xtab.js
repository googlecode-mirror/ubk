/**
 * Contiene, per ogni pagina di proprietà, il numero di pagina correntemente attivato. Le pagine sono identificate dal loro progressivo univoco.
 */
var tabstrip_corrente = new Array();

/**
 * Attiva la pagina relativa all'intestazione cliccata.
 * @param cella object La cella contenente la scritta di intestazione
 * @param indice integer Il numero di pagina da attivare
 * @param contenuto string Il numero progressivo identificativo della pagina di proprietà entro la pagina HTML
 * @param classe string La classe css della tabella
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function tabstrip_roll(cella)
{
	var tb = trova_padre(cella, "TABLE");
	var a = cella.id.split('_');
	var indice = a[2];
	var contenuto = a[1];
	var classe = tb.className;

	if (tabstrip_corrente[contenuto] == indice) return;
	
	var cella_centrale = document.getElementById("tabstripcontent_" + contenuto);

	
	if (tabstrip_corrente[contenuto] != null) {

		/* passo 1: disattivo i contenuti */
		cella_centrale.childNodes[tabstrip_corrente[contenuto]].style.display = "none";

		/* passo 2: disattivo l'intestazione corrente */
		var cella_vecchia = cella.parentNode.childNodes[1 + 2 * tabstrip_corrente[contenuto]];
		if (cella_vecchia.previousSibling.className == classe + "_tl1") {
			cella_vecchia.previousSibling.className = classe + "_tl0";
		} else if (tabstrip_corrente[contenuto] == indice + 1) {
			cella_vecchia.previousSibling.className = classe + "_tr10";
		} else {
			cella_vecchia.previousSibling.className = classe + "_tr00";
		}
		cella_vecchia.className = classe + "_t0";
		if (cella_vecchia.nextSibling.className == classe + "_trf1") {
			cella_vecchia.nextSibling.className = classe + "_trf0";
			cella_vecchia.nextSibling.nextSibling.className = classe + "_trp0";
			cella_vecchia.nextSibling.nextSibling.nextSibling.className = classe + "_tr0";
		} else if (tabstrip_corrente[contenuto] == indice - 1) {
			cella_vecchia.nextSibling.className = classe + "_tr01";
		} else {
			cella_vecchia.nextSibling.className = classe + "_tr00";
		}
	}

	/* passo 3: attivo il nuovo contenuto */
	cella_centrale.childNodes[indice].style.display = "block";

	/* passo 4: attivo l'intestazione su cui ho cliccato */
	if (cella.previousSibling.className == classe + "_tl0" || cella.previousSibling.className == classe + "_tl1" ) {
		cella.previousSibling.className = classe + "_tl1";
	} else {
		cella.previousSibling.className = classe + "_tr01";
	}
	cella.className = classe + "_t1";
	if (cella.nextSibling.className == classe + "_trf0") {
		cella.nextSibling.className = classe + "_trf1";
		cella.nextSibling.nextSibling.className = classe + "_trp1";
		cella.nextSibling.nextSibling.nextSibling.className = classe + "_tr1";
	} else {
		cella.nextSibling.className = classe + "_tr10";
	}	

	/* passo 5: seguo il link se esiste */
	var link = trova_figlio_completo(cella, 'A');

	if (link != null) {

		if (link.onclick != null) 
			link.onclick.call(link);

		if (link.href != '#')
			window.frames[link.target].document.location = link.href;
	}

	// aggiorno la tabstrip corrente
	tabstrip_corrente[contenuto] = indice;
}

function tabstrip_force(indice, contenuto)
{
	if (tabstrip_corrente[contenuto] == indice)
		tabstrip_corrente[contenuto] = null;
	var hd = document.getElementById('tabstriphead_' + contenuto + '_' + indice);
	tabstrip_roll(hd);
}

function tabstrip_restore(contenuto, larghezza)
{
	return; 
	var hd = document.getElementById('tabstriphead_' + contenuto + '_0');
	var tb = trova_padre(hd, "TABLE");
	tb.style.width = larghezza;
}

function tabstrip_deactivate(indice, contenuto)
{
	var hd = document.getElementById('tabstriphead_' + contenuto + '_' + indice);
	var hd_parent = hd.parentNode;
	var trp = hd_parent.childNodes[hd_parent.childNodes.length - 2];
	var tb = trova_padre(hd, "TABLE");

	var cella_centrale = document.getElementById("tabstripcontent_" + contenuto);
	cella_centrale.removeChild(cella_centrale.childNodes[indice]);

	// se ranzo la tab attiva, nn c'e' + contenuto corrente
	if (tabstrip_corrente[contenuto] == indice) {
		tabstrip_corrente[contenuto] = null;
	}
	// da lei in poi, i tabstrip_head e i tabstrip_content devono essere rinominati
	for(var i = 1 + 2 * (indice + 1); i < hd_parent.childNodes.length - 2; i += 2) {
		var idx = (i - 1) / 2 - 1;
		var bros = hd_parent.childNodes[i];
		var cont = cella_centrale.childNodes[idx];

		bros.id = 'tabstriphead_' + contenuto + '_' + (idx);
		cont.id = 'tabstripcontent_' + contenuto + '_' + (idx);
	}


	var prv_cls = hd.previousSibling.className;
	var nxt_cls = hd.nextSibling.className;

	var prv_cls_name = prv_cls.substring(prv_cls.lastIndexOf('_') + 1);
	var nxt_cls_name = nxt_cls.substring(nxt_cls.lastIndexOf('_') + 1);

	// sono sulla prima tab - quindi devo ranzare la cella e suo fratello dx
	if (prv_cls_name == "tl1" || prv_cls_name == "tl0") {
		trp.style.width = trp.offsetWidth + hd.offsetWidth + hd.nextSibling.offsetWidth;
		hd_parent.removeChild(hd.nextSibling);
		hd_parent.removeChild(hd);
	// sono sull'ultima prima tab - quindi devo ranzare la cella e suo fratello sx
	} else if (nxt_cls_name == "trf0" || nxt_cls_name == "trf1") {
		trp.style.width = trp.offsetWidth + hd.offsetWidth + hd.previousSibling.offsetWidth;
		hd_parent.removeChild(hd.previousSibling);
		hd_parent.removeChild(hd);

	// in una centrale è indifferente
	} else {
		trp.style.width = trp.offsetWidth + hd.offsetWidth + hd.nextSibling.offsetWidth;
		hd_parent.removeChild(hd.nextSibling);
		hd_parent.removeChild(hd);
	}

	var colspan = cella_centrale.getAttribute('colspan') - 2;
	cella_centrale.setAttribute('colSpan',colspan);
	
	var bordo_inf_centrale = document.getElementById("tabstripbottom_" + contenuto);
	bordo_inf_centrale.setAttribute('colSpan',colspan);

	tabstrip_force(0, contenuto);
}

function tabstrip_head_over(event, cella)
{
	var a = cella.id.split('_');
	var indice = a[2];
	var contenuto = a[1];

	if (tabstrip_corrente[contenuto] == indice) return; 
	var el = event.srcElement == null ? event.target : event.srcElement;
	el.style.textDecoration = 'underline';
}

function tabstrip_head_out(event, cella, color)
{
	var el = event.srcElement == null ? event.target : event.srcElement;
	el.style.textDecoration = 'none'; 
	if (color != null) {
		var a = cella.id.split('_');
		var indice = a[2];
		var contenuto = a[1];
		if (tabstrip_corrente[contenuto] != indice) {
			el.style.color = color;
		}
	}
}

function tabstrip_head_off(cella, color)
{
	var a = cella.id.split('_');
	var contenuto = a[1];

	var cella_vecchia = cella.parentNode.childNodes[1 + 2 * tabstrip_corrente[contenuto]];
	if (cella != cella_vecchia)
		cella_vecchia.firstChild.style.color = color;
}

function tabstrip_set_color(event, color)
{
	var el = event.srcElement == null ? event.target : event.srcElement;
	el.style.color = color;
}