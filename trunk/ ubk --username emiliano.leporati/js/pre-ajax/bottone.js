/**
 * Tiene traccia del bottone su cui attualmente è avvenuto il click
 */
var bottone_cliccato = null;
/**
 * Serve per ricomporre la classe del link contenuto nel bottone al momento del roll-out, cui sul roll-over viene aggiunta "hover"
 */
var classe_bottone_cliccato = "";

/**
 * Rolla il bottone all'ingresso del mouse; se è il bottone attualmente cliccato non fa nulla
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function roll_bottone_over(tabella, classe)
{
	if (bottone_cliccato == tabella) return;
	tabella.previousSibling.className = classe + "_l_hover";
	tabella.className = classe + "_c_hover";
	tabella.nextSibling.className = classe + "_r_hover";
	if (tabella.firstChild.tagName == "A") {
		classe_bottone_cliccato = tabella.firstChild.className;
		tabella.firstChild.className += " hover";
	}
}

/**
 * Rolla il bottone all'uscita del mouse; se è il bottone attualmente cliccato non fa nulla
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function roll_bottone_out(tabella, classe)
{
	if (bottone_cliccato == tabella) return;
	tabella.previousSibling.className = classe + "_l";
	tabella.className = classe + "_c";
	tabella.nextSibling.className = classe + "_r";
	if (tabella.firstChild.tagName == "A") {
		tabella.firstChild.className = classe_bottone_cliccato;
	}
}

/**
 * Rolla in uscita il bottone attualmente cliccato, se ve n'è uno, e imposta come cliccato quello passato, in modo che i roll-over e out non abbiano più effetto su di esso
 * @author Ubik <leporati@immaginazioneelavoro.it>
 */
function roll_bottone_click(tabella, classe)
{
	if (bottone_cliccato == tabella) return;
	if (bottone_cliccato != null) {
		var old_bottone = bottone_cliccato;
		bottone_cliccato = null;
		roll_bottone_out(old_bottone, classe);
	}
	bottone_cliccato = tabella;
}
