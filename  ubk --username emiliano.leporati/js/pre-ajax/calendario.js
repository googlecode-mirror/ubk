var mese_corrente = null;
var anno_corrente = null;
var nome_mese = Array("Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre");

function mese_seleziona(mese)
{
	if (mese_corrente != null) {
		document.getElementById("mese_" + mese_corrente).style.display = 'none';
	}
	mese_corrente = mese;

	var idMese = document.getElementById('mese_id');
	idMese.value = mese + 1;

	var nomeMese = document.getElementById('mese_nome');
	nomeMese.value = nome_mese[mese];
	
	var divMese = document.getElementById("mese_" + mese);
	divMese.style.display = 'block';

	var divCombo = divMese.parentNode.previousSibling;
	var table = divCombo.firstChild;
	var tr = trova_figlio(table, "TR");
	var label_table = tr.nextSibling.childNodes[1];
	var label = trova_figlio(label_table, "DIV");
	label.innerHTML = nome_mese[mese];
	label.style.cursor = "default";
}

function mese_successivo(gateway, file)
{
	if (mese_corrente < 11) {
		document.getElementById("mese_" + mese_corrente).style.display = 'none';
		document.getElementById("mese_" + (++ mese_corrente)).style.display = 'block';
		mese_seleziona(mese_corrente);
		return true;
	} else {
		document.location = gateway + '?FILE=' + file + '&AZIONE=mostra_anno&MESE=0&ANNO=' + (anno_corrente + 1);
	}
}

function mese_precedente(gateway, file)
{
	if(mese_corrente > 0){
		document.getElementById("mese_" + mese_corrente).style.display = 'none';
		document.getElementById("mese_" + (-- mese_corrente)).style.display = 'block';
		mese_seleziona(mese_corrente);
		return true;
	} else {
		document.location = gateway + '?FILE=' + file + '&AZIONE=mostra_anno&MESE=11&ANNO=' + (anno_corrente - 1);
	}
}