<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


	<xsl:template match="gfx:combo">

		<xsl:if test="not(@class)">
			<xsl:message terminate="yes">Manca attributo "class" in tag "gfx:combo".</xsl:message>
		</xsl:if>

		<xsl:variable name="class" select="@class"/>
		
		<xsl:element name="table">
			<xsl:attribute name="class"><xsl:value-of select="concat($class, '_head closed')"/></xsl:attribute>
			<xsl:element name="tbody">
				<xsl:element name="tr">
					<xsl:element name="td">
						<xsl:element name="div">
							<!--<xsl:attribute name="id">combo_label</xsl:attribute>-->
							<xsl:attribute name="class"><xsl:value-of select="concat($class, '_items')"/></xsl:attribute>
							<xsl:value-of select="gfx:label|gfx:LABEL"/>
						</xsl:element>
					</xsl:element>
					<xsl:element name="td">
						<xsl:attribute name="class"><xsl:value-of select="concat($class, '_tdImg closed')"/></xsl:attribute>
						<xsl:attribute name="onclick">comboToggleFromDDImg(this);</xsl:attribute>
						<img src="./img/DownDis.gif" border="0" onmouseover="rollaOver(this);" onmouseout="rollaOut(this);"/>
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:element>
		<xsl:element name="div">
			<xsl:attribute name="style">display: none;</xsl:attribute>
			<xsl:attribute name="class"><xsl:value-of select="concat($class, '_list closed')"/></xsl:attribute>
			<xsl:apply-templates select="gfx:items|gfx:ITEMS"/>
		</xsl:element>

	</xsl:template>

	<xsl:template match="gfx:items">
		<xsl:variable name="class" select="../@class"/>
		<xsl:for-each select="gfx:ITEM|gfx:item">
			<xsl:element name="div">
				<xsl:attribute name="class"><xsl:value-of select="concat($class, '_items')"/></xsl:attribute>
				<xsl:attribute name="onclick"><xsl:value-of select="@onclick"/>; this.className = '<xsl:value-of select="$class"/>_items'; closeCombo(this);</xsl:attribute>
				<xsl:attribute name="onmouseover">this.className = '<xsl:value-of select="$class"/>_items over'; stop_event_propagation(event);</xsl:attribute>
				<xsl:attribute name="onmouseout">this.className = '<xsl:value-of select="$class"/>_items'; stop_event_propagation(event);</xsl:attribute>
				<xsl:value-of select="item/text()"/>
				<xsl:apply-templates/>
			</xsl:element>
		</xsl:for-each>
	</xsl:template>


</xsl:stylesheet>