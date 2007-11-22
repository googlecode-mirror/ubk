<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template match="gfx:button">

	<xsl:if test="not(@class)">
		<xsl:message terminate="yes">Manca attributo "class" in tag "gfx:button".</xsl:message>
	</xsl:if>

	<!-- tabella -->
	<table class="{@class}">
		<tr>
			<td class="{concat(@class, '_l')}"/>
			<td class="{concat(@class, '_c')}" onmouseover="roll_bottone_over(this, '{@class}')" onmouseout="roll_bottone_out(this, '{@class}')">
				<xsl:if test="@click = 'true'">
					<xsl:attribute name="onclick">roll_bottone_click(this, '<xsl:value-of select="@class"/>')</xsl:attribute>
				</xsl:if>

				<xsl:apply-templates/>
			</td>
			<td class="{concat(@class, '_r')}"/>
		</tr>
	</table>

 </xsl:template>

</xsl:stylesheet>