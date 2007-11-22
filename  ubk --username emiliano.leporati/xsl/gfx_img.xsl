<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">

<!-- ************************************************************** -->

<!--               T E M P L A T E                                  -->

<!-- ************************************************************** -->
<xsl:template match="gfx:img">

	<xsl:if test="not(@nome)">
		<xsl:message terminate="yes">Manca attributo "nome" in tag "image".</xsl:message>
	</xsl:if>

	<xsl:variable name="name" select="@nome"/>

	<xsl:variable name="ext">
		<xsl:choose>
			<xsl:when test="@est"><xsl:value-of select="@est"/></xsl:when>
			<xsl:when test="$def-img-ext"><xsl:value-of select="$def-img-ext"/></xsl:when>
			<xsl:otherwise>.gif</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="path">
		<xsl:choose>
			<xsl:when test="@path"><xsl:value-of select="@path"/></xsl:when>
			<xsl:otherwise>img/</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="alt">
		<xsl:choose>
			<xsl:when test="@alt"><xsl:value-of select="@alt"/></xsl:when>
			<xsl:otherwise><xsl:value-of select="document('alt_defaults.xml')//alt[@image = $name]/@alt"/></xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="onmsover">
		<xsl:choose>
			<xsl:when test="@onmouseover"><xsl:value-of select="@onmouseover"/>;img_over(this);</xsl:when>
			<xsl:otherwise>img_over(this);</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="onmsout">
		<xsl:choose>
			<xsl:when test="@onmouseout"><xsl:value-of select="@onmouseout"/>;img_out(this);</xsl:when>
			<xsl:otherwise>img_out(this);</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>


	<xsl:element name="img">
		<xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute>
		<xsl:attribute name="alt"><xsl:value-of select="$alt"/></xsl:attribute>
		<xsl:attribute name="title"><xsl:value-of select="$alt"/></xsl:attribute>
		<xsl:attribute name="src"><xsl:value-of select="concat($path, $name, 'Dis', $ext)"/></xsl:attribute>
		<xsl:if test="not(@roll = 'false')">
			<xsl:attribute name="onmouseover"><xsl:value-of select="$onmsover"/></xsl:attribute>
			<xsl:attribute name="onmouseout"><xsl:value-of select="$onmsout"/></xsl:attribute>
		</xsl:if>
	</xsl:element>

</xsl:template>


<!-- N A M E D    T E M P L A T E -->
<xsl:template name="gfx-img">
	<xsl:param name="nome"/>
	<xsl:param name="class"/>
	<xsl:param name="est"/>
	<xsl:param name="path"/>
	<xsl:param name="alt"/>
	<xsl:param name="onmouseover"/>
	<xsl:param name="onmouseout"/>

	<xsl:variable name="local-est">
		<xsl:choose>
			<xsl:when test="$est"><xsl:value-of select="$est"/></xsl:when>
			<xsl:when test="$def-img-ext"><xsl:value-of select="$def-img-ext"/></xsl:when>
			<xsl:otherwise>.gif</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="local-path">
		<xsl:choose>
			<xsl:when test="$path"><xsl:value-of select="$path"/></xsl:when>
			<xsl:otherwise>img/</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="local-alt">
		<xsl:choose>
			<xsl:when test="$alt"><xsl:value-of select="$alt"/></xsl:when>
			<xsl:otherwise><xsl:value-of select="document('alt_defaults.xml')//alt[@image = $nome]/@alt"/></xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="onmsover">
		<xsl:choose>
			<xsl:when test="@onmouseover"><xsl:value-of select="@onmouseover"/>;img_over(this);</xsl:when>
			<xsl:otherwise>img_over(this);</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="onmsout">
		<xsl:choose>
			<xsl:when test="@onmouseout"><xsl:value-of select="@onmouseout"/>;img_out(this);</xsl:when>
			<xsl:otherwise>img_out(this);</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:element name="img">
		<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
		<xsl:attribute name="alt"><xsl:value-of select="$local-alt"/></xsl:attribute>
		<xsl:attribute name="title"><xsl:value-of select="$local-alt"/></xsl:attribute>
		<xsl:attribute name="src"><xsl:value-of select="concat($local-path, $nome, 'Dis', $local-est)"/></xsl:attribute>
		<xsl:if test="not(@roll = 'false')">
			<xsl:attribute name="onmouseover"><xsl:value-of select="$onmsover"/></xsl:attribute>
			<xsl:attribute name="onmouseout"><xsl:value-of select="$onmsout"/></xsl:attribute>
		</xsl:if>
	</xsl:element>

</xsl:template>

</xsl:stylesheet>