<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template match="gfx:page-lite">

	<html>
		<head>
			<title><xsl:value-of select="@titolo"/></title>
			<link rel="stylesheet" type="text/css" href="css/stile.css"/>
			<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
			<script language="javascript" src="/risorse/js/bottone.js"></script>
			<script language="javascript" src="/risorse/js/varie.js"></script>
 			<xsl:apply-templates select="*[name() = 'style' or name() = 'link' or name() = 'base']"/>
		</head>
		<xsl:element name="body">
			<xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute>
			<xsl:attribute name="onload"><xsl:value-of select="@onload"/></xsl:attribute>
			<xsl:attribute name="onunload"><xsl:value-of select="@onunload"/></xsl:attribute>

			<!-- Trasformazione del contenuto -->
			<xsl:apply-templates select="*[name() != 'style' and name() != 'link' and name() != 'base']"/>

		</xsl:element>
	</html>

 </xsl:template>

</xsl:stylesheet>