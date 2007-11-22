<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template match="gfx:trim">

	<xsl:if test="not(@larghezza)">
		<xsl:message terminate="yes">Manca attributo "larghezza" in tag "trim".</xsl:message>
	</xsl:if>
	
	<xsl:element name="div">
		<xsl:attribute name="class">trim <xsl:value-of select="@class"/></xsl:attribute>
		<xsl:choose>
			<xsl:when test="@altezza">
				<xsl:attribute name="style">white-space: normal; width: <xsl:value-of select="@larghezza"/>; height: <xsl:value-of select="@altezza"/></xsl:attribute>
			</xsl:when>
			<xsl:otherwise>
				<xsl:attribute name="style">white-space: nowrap; width: <xsl:value-of select="@larghezza"/>;</xsl:attribute>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:choose>
			<xsl:when test="not(@without-alt = 'true')">				
				<xsl:attribute name="onmouseover">trim_show(event)</xsl:attribute>
				<xsl:attribute name="onmouseout">trim_hide(event)</xsl:attribute>
				<xsl:attribute name="onmousemove">trim_move(event)</xsl:attribute>
			</xsl:when>
		</xsl:choose>
			
		<xsl:apply-templates select="text()|*[name()!='gfx:label']"/>

	</xsl:element>
	<xsl:if test="gfx:label">
		<div style="display: none">
			<xsl:apply-templates select="gfx:label/*|gfx:label/text()"/>
		</div>
	</xsl:if>


</xsl:template>

</xsl:stylesheet>