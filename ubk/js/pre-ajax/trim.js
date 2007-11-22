function set_position(o, new_x, new_y)
{
/*	window.status = "left: " + new_x + " top: " + new_y;
	window.status += " width: " + o.offsetWidth + " height: " + o.offsetHeight;
	window.status += " b_width: " + document.body.offsetWidth + " b_height: " + document.body.offsetHeight;
*/

	var scroll_x, scroll_y;
	if (window.event == null) {
		scroll_x = window.scrollX;
		scroll_y = window.scrollY;
	} else {
		scroll_x = document.body.scrollLeft;
		scroll_y = document.body.scrollTop;

		if (o.offsetWidth + new_x > document.body.clientWidth + scroll_x)
			new_x = (document.body.clientWidth - o.clientWidth - 3 + scroll_x);
		if (o.offsetHeight + new_y > document.body.clientHeight + scroll_y)
			new_y = (document.body.clientHeight - o.clientHeight - 3 + scroll_y);
	}

	
	o.style.left = new_x;
	o.style.top  = new_y;

//	window.status = document.body.clientWidth + " " + scroll_y;
}
/**
 * Mostra il contenuto completo di uno SPAN trimmato
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function trim_show(e)
{
	var o = document.getElementById('trim_viewer');
	var new_x, new_y, src;

	var src = (e.target || window.event.srcElement);
	if (src.nextSibling && src.nextSibling.tagName == 'DIV' && src.nextSibling.style.display == 'none')
		src = src.nextSibling;

	if (src.innerHTML.length >= 2) {
		o.innerHTML = src.innerHTML;
		if (window.event == null) {
			new_x = e.clientX + 15;
			new_y = e.clientY + 15;
		} else {
			new_x = document.body.scrollLeft + window.event.clientX + 15;
			new_y = document.body.scrollTop  + window.event.clientY + 15;
		}
	
		set_position(o, new_x, new_y);
		o.style.visibility = 'visible';
		if (src.previousSibling)
			src.previousSibling.style.cursor = 'help';
	}
}

/**
 * Nasconde la pop-up di visualizzazione completa
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function trim_hide(e)
{
	var src = (window.event == null) ? e.target : window.event.srcElement;

	document.getElementById('trim_viewer').style.visibility = 'hidden';
	src.style.cursor = 'default';
}

/**
 * Sposta la pop-up in base alla posizione del mouse
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function trim_move(e)
{
	var o = document.getElementById('trim_viewer');
	var new_x, new_y;
	if (window.event == null) {
		new_x = window.scrollX + e.clientX + 15;
		new_y = window.scrollY + e.clientY + 15;
	} else {
		new_x = document.body.scrollLeft + window.event.clientX + 15;
		new_y = document.body.scrollTop  + window.event.clientY + 15;
	}

	set_position(o, new_x, new_y);
}
