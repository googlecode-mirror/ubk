<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template match="doc">

	<xsl:variable name="file">
		<xsl:choose>
			<xsl:when test="not(@file)">doc.xml</xsl:when>
			<xsl:otherwise><xsl:value-of select="@file"/></xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="path" select="@path"/>

	<xsl:variable name="doc" select="concat($phdir, 'xsl/', $file)"/>

	<xsl:apply-templates/>

	<div class="doc_div">
		<xsl:value-of select="document($doc)/doc/doc[@path = $path]"/>
	</div>

</xsl:template>



</xsl:stylesheet>