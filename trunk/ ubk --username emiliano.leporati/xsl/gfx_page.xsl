<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">

<xsl:template match="gfx:page[name(..) != 'gfx:property-page']">

	<html>
		<head>
			<title>
				<xsl:choose>
					<xsl:when test="gfx:titolo">
						<xsl:apply-templates select="gfx:titolo/*|gfx:titolo/text()"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="@titolo"/>
					</xsl:otherwise>
				</xsl:choose>
			</title>
			<link rel="stylesheet" type="text/css" href="css/stile.css"/>
			<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
			<script language="javascript" src="/ubk/js/check.js"></script>
			<script language="javascript" src="/ubk/js/bottone.js"></script>
			<script language="javascript" src="/ubk/js/xtab.js"></script>
			<script language="javascript" src="/ubk/js/trim.js"></script>
			<script language="javascript" src="/ubk/js/varie.js"></script>
			<script language="javascript" src="/ubk/js/calendario.js"></script>
			<script language="javascript" src="/ubk/js/xcombo.js"></script>
			<script language="Javascript">
				<xsl:choose>
					<xsl:when test="@help">
						help_page = '<xsl:value-of select="@help"/>';
					</xsl:when>
					<xsl:otherwise>
						help_page = null;
					</xsl:otherwise>
				</xsl:choose>

				<xsl:element name="ubk:var">$_SESSION['i_acc_gruppi_utente_id_js']</xsl:element>
			</script>
			<style type="text/css">
				.doc_div {
					visibility: hidden; 
					display: none;
				}
				.trim {
					float: left; 
					overflow: hidden; 
					clip: auto; 
					text-overflow: ellipsis; 
					font-size: 8pt;
				}
			</style>
 			<xsl:apply-templates select="*[name() = 'style' or name() = 'link' or name() = 'base']"/>
		</head>
		<xsl:element name="body">
			<xsl:attribute name="style"><xsl:value-of select="@style"/></xsl:attribute>
			<xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute>
			<xsl:attribute name="onload"><xsl:value-of select="@onload"/></xsl:attribute>
			<xsl:attribute name="onunload"><xsl:value-of select="@onunload"/></xsl:attribute>
			<xsl:attribute name="onblur"><xsl:value-of select="@onblur"/></xsl:attribute>
			<xsl:attribute name="onfocus"><xsl:value-of select="@onfocus"/></xsl:attribute>
			<!-- serve a creare l'attributo oncontextmenu con codice js per disabilitare il tasto destro del mouse -->
			<xsl:if test="@disabilita-tasto-dx = 'true'">
				<xsl:attribute name="oncontextmenu">return false;</xsl:attribute>
			</xsl:if>
			<xsl:if test="@align">
				<xsl:attribute name="style">text-align: <xsl:value-of select="@align"/></xsl:attribute>
			</xsl:if>
			<!-- <xsl:if test="@modale = 'true'">
				<xsl:attribute name="onblur">window.focus()</xsl:attribute>
			</xsl:if> -->
			<xsl:if test="@h1 = 'true'">
				<h1>
					<xsl:choose>
						<xsl:when test="gfx:titolo">
							<xsl:apply-templates select="gfx:titolo/*|gfx:titolo/text()"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="@titolo"/>
						</xsl:otherwise>
					</xsl:choose>
				</h1>
			</xsl:if>

			<!-- Visualizzazione espansa dei TRIM -->
			<div id="trim_viewer" style="display: inline; visibility: hidden; position: absolute; z-index: 50;">
			</div>

			<!-- Trasformazione del contenuto -->
			<xsl:apply-templates select="*[name() != 'style' and name() != 'link' and name() != 'base' and name() != 'gfx:titolo']|text()"/>


		</xsl:element>
	</html>

 </xsl:template>

</xsl:stylesheet>