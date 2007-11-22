/**
 * quando clicco sull'immagine, faccio il toggle della visibilità della lista
 */

function comboToggleFromDDImg(el)
{
	var drop_table = trova_padre(el, "TABLE");
	var cont_row   = trova_padre(drop_table, "TR");
	var cont_table = trova_padre(drop_table, "TABLE");
	var combo = drop_table.nextSibling;

	for(var cont_div = cont_table.parentNode; cont_div.tagName == "DIV" && cont_div.parentNode.tagName == "DIV" ; cont_div = cont_div.parentNode);

	combo.style.top  = drop_table.offsetHeight + drop_table.offsetTop + cont_div.offsetTop + cont_row.offsetTop + 1 ;
	combo.style.left = drop_table.offsetLeft + drop_table.parentNode.offsetLeft + cont_table.offsetLeft + cont_div.offsetLeft + 1;

	if (combo.style.display == 'none') {
		combo.style.display = 'block';
		rollaOver(el.firstChild);
	} else {
		combo.style.display = 'none';
		rollaOut(el.firstChild);
	}
}

function rollaOver(el)
{
	// over su immagine
	if (el.src.lastIndexOf("Dis.gif") != -1) {
		el.src = el.src.substr(0, el.src.lastIndexOf("Dis.gif")) + ".gif";
	}

	// over su cella
	var td_class = el.parentNode.className;
	var the_class = td_class.substring(0, td_class.lastIndexOf('_tdImg'));

	var table = trova_padre(el, "TABLE");
	var list  = table.nextSibling;

	el.parentNode.className =  the_class + "_tdImg rollover";
	table.className = the_class + "_head rollover";
	list.className  = the_class + "_list rollover";

}

function rollaOut(el)
{
	// out su immagine
	if (el.src.lastIndexOf("Dis.gif") == -1) {
		el.src = el.src.substr(0, el.src.lastIndexOf('.')) + "Dis.gif";
	}

	var td_class = el.parentNode.className;
	var the_class = td_class.substring(0, td_class.lastIndexOf('_tdImg '));

	var table = trova_padre(el, "TABLE");
	var list  = table.nextSibling;
	
	el.parentNode.className = the_class + '_tdImg closed';

	if (list.style.display == "none") {
		table.className = the_class + "_head closed";
		list.className  = the_class + "_list closed";
	} else {
		table.className = the_class + "_head open";
		list.className  = the_class + "_list open";
	}


}

function closeCombo(comboVoice)
{
	var div = trova_padre(comboVoice, "DIV");
	var table = div.parentNode;
	var tr = trova_figlio(table, "TR");
	comboToggleFromDDImg(tr.childNodes[1]);	// cella centrale
}