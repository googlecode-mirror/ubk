<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<!-- ************************************************************** -->

<!--               T E M P L A T E                                  -->

<!-- ************************************************************** -->
<xsl:template match="gfx:include">

	<xsl:if test="not(@file)">
		<xsl:message terminate="yes">Manca attributo "file" in tag "gfx:include".</xsl:message>
	</xsl:if>

	<xsl:variable name="path" select="concat($phdir,@file)"/>

	<xsl:apply-templates select="document($path)"/>

</xsl:template>




<!-- N A M E D    T E M P L A T E -->
<xsl:template name="gfx-include">
	<xsl:param name="file"/>

	<xsl:if test="not($file)">
		<xsl:message terminate="yes">Omesso attributo "file" nella chiamata a "gfx-include".</xsl:message>
	</xsl:if>

	<xsl:variable name="path" select="concat($phdir,$file)"/>

	<xsl:apply-templates select="document($path)"/>
</xsl:template>

</xsl:stylesheet>