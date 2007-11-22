<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template match="gfx:frame">

	<xsl:if test="not(@class)">
		<xsl:message terminate="yes">Manca attributo richiesto 'class' in tag 'xframe'.</xsl:message>
	</xsl:if>

	<div class="{@class}" style="height: {@altezza}; width: {@larghezza}">

		<xsl:if test="boolean(@titolo)">
			<div class="{concat(@class, '_titolo_contenitore')}">
				<span class="{concat(@class, '_titolo')}">
					<span class="{concat(@class, '_titolo_on')}" style="position: relative">
						<xsl:choose>
							<xsl:when test="substring(@titolo, 1, 1) = '$'">
								<xsl:element name="ubk:var"><xsl:value-of select="@titolo"/></xsl:element>
							</xsl:when>
							<xsl:otherwise>
								<xsl:value-of select="@titolo"/>
							</xsl:otherwise>
						</xsl:choose>
					</span>
				</span>
			</div>
		</xsl:if>

		<xsl:apply-templates/>

	</div>

</xsl:template>

<!-- <xsl:variable name="shadow">
	<xsl:choose>
		<xsl:when test="@shadow"><xsl:value-of select="@shadow"/></xsl:when>
		<xsl:otherwise>true</xsl:otherwise>
	</xsl:choose>
</xsl:variable>


<xsl:element name="div">
	<xsl:attribute name="class"><xsl:value-of select="@class"/></xsl:attribute>
	<xsl:attribute name="style">height: <xsl:value-of select="@altezza"/>; width: <xsl:value-of select="@larghezza"/>;</xsl:attribute>

	<xsl:if test="boolean(@titolo)">

		<xsl:element name="div">
			<xsl:attribute name="class"><xsl:value-of select="concat(@class, '_titolo_contenitore')"/></xsl:attribute>

			<xsl:element name="span">
				<xsl:attribute name="class"><xsl:value-of select="concat(@class, '_titolo')"/></xsl:attribute>

				<xsl:if test="$shadow = 'true'">
					<xsl:element name="span">
						<xsl:attribute name="class"><xsl:value-of select="concat(@class, '_titolo_shadow')"/></xsl:attribute>
						<xsl:value-of select="@titolo"/>
					</xsl:element>
				</xsl:if>

				<xsl:element name="span">
					<xsl:attribute name="class"><xsl:value-of select="concat(@class, '_titolo_on')"/></xsl:attribute>
					<xsl:attribute name="style">position: relative;</xsl:attribute>
					<xsl:value-of select="@titolo"/>
				</xsl:element>
			
			</xsl:element>

		</xsl:element>

	</xsl:if>

	<xsl:apply-templates/>

</xsl:element>

</xsl:template>
 -->

</xsl:stylesheet>