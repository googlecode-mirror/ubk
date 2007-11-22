<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<!-- 

ATTRIBUTI:
class
numero
larghezza
altezza

-->

<xsl:template match="gfx:property-page">

	<xsl:if test="not(@class)">
		<xsl:message terminate="yes">Manca attributo "class" in tag "gfx:property-page".</xsl:message>
	</xsl:if>

	<xsl:if test="not(@numero)">
		<xsl:message terminate="yes">Manca attributo "numero" in tag "gfx:property-page".</xsl:message>
	</xsl:if>


	<xsl:variable name="numero-pagine" select="count(gfx:page)"/>
	<xsl:variable name="class" select="@class"/>
	<xsl:variable name="numero" select="@numero"/>
	
	<SCRIPT language="Javascript">
		tabstrip_corrente['<xsl:value-of select="@numero"/>'] = 0; 
	</SCRIPT>

	<xsl:element name="table">
		<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
		<xsl:attribute name="style">border:0; border-collapse: collapse; empty-cells: show; width: <xsl:value-of select="@larghezza"/>; height: <xsl:value-of select="@altezza"/>; table-layout: fixed;</xsl:attribute>

		<!-- riga con le intestazioni -->
		<tr>
			<td class="{concat($class,'_tl1')}"/>

			<xsl:for-each select="gfx:page/gfx:head">
				<xsl:variable name="th-class">
					<xsl:choose>
						<xsl:when test="position() = 1">_t1</xsl:when>
						<xsl:otherwise>_t0</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>

				<xsl:variable name="js-over">
					tabstrip_head_over(event, this);
				</xsl:variable>

				<xsl:variable name="js-out">
					<xsl:choose>
						<xsl:when test="../../@out-color">
							tabstrip_head_out(event, this, '<xsl:value-of select="../../@out-color"/>');
						</xsl:when>
						<xsl:otherwise>
							tabstrip_head_out(event, this, null);
						</xsl:otherwise>
					</xsl:choose>
				</xsl:variable>

				<xsl:variable name="js-click">
					<xsl:if test="../../@over-color">
						tabstrip_set_color(event, '<xsl:value-of select="../../@over-color"/>');
					</xsl:if>
					<xsl:if test="../../@out-color">
						tabstrip_head_off(this.parentNode, '<xsl:value-of select="../../@out-color"/>');
					</xsl:if>
					tabstrip_roll(this); 
					<xsl:if test="@js-onclick">
						<xsl:value-of select="@js-onclick"/>
					</xsl:if>
				</xsl:variable>

				<th class="{concat($class, $th-class)}" id="{concat('tabstriphead_', $numero, '_', position() - 1)}" onclick="{normalize-space($js-click)}" onmouseover="{normalize-space($js-over)}" onmouseout="{normalize-space($js-out)}">
					<xsl:if test="@larghezza">
						<xsl:attribute name="style">width: <xsl:value-of select="@larghezza"/></xsl:attribute>
					</xsl:if>

					<xsl:choose>
						<xsl:when test="@disabled-on-w='true'">
							<xsl:element name="ubk:if">
								<xsl:element name="ubk:test">$parametri['GESTORE']->eof</xsl:element>
								<xsl:element name="ubk:then">
									<span class="{concat($class, '_disab')}">
										<xsl:apply-templates select=".//text()"/>
									</span>
								</xsl:element>
								<xsl:element name="ubk:else">
									<span>
										<xsl:apply-templates/>	
									</span>
								</xsl:element>
							</xsl:element>
						</xsl:when>
						<xsl:otherwise>
							<span>
								<xsl:apply-templates/>	
							</span>
						</xsl:otherwise>
					</xsl:choose>

				</th>


				<xsl:choose>
					<xsl:when test="position() = $numero-pagine and position() = 1">
						<td class="{concat($class, '_trf1')}"/>
						<td class="{concat($class, '_trp1')}"/>
						<td class="{concat($class, '_tr1')}"/>
					</xsl:when>
					<xsl:when test="position() = $numero-pagine">
						<td class="{concat($class, '_trf0')}"/>
						<td class="{concat($class, '_trp0')}"/>
						<td class="{concat($class, '_tr0')}"/>
					</xsl:when>
					<xsl:when test="position() = 1">
						<td class="{concat($class, '_tr10')}"/>
					</xsl:when>
					<xsl:otherwise>
						<td class="{concat($class, '_tr00')}"/>
					</xsl:otherwise>
				</xsl:choose>

			</xsl:for-each>
		</tr>


		<!-- contenuti -->
		<tr>
			<td class="{concat($class, '_l')}"/>
			<td class="{$class}" id="{concat('tabstripcontent_', $numero)}" colspan="{2 * $numero-pagine + 1}">
				<xsl:for-each select="gfx:page/gfx:body">
					<xsl:element name="div">
						<xsl:attribute name="id"><xsl:value-of select="concat('tabstripcontent_', $numero, '_', position() - 1)"/></xsl:attribute>
						<xsl:attribute name="style">
							<xsl:variable name="altezza" select="(../../@altezza)"/>
							<xsl:variable name="um-altezza" select="substring($altezza, string-length($altezza) - 1)"/>
							<xsl:variable name="altezza-reale">
								<xsl:choose>
									<xsl:when test="number($altezza) != NaN">
										<xsl:value-of select="number($altezza) - 20"/>
									</xsl:when>
									<xsl:when test="$um-altezza = 'px'">
										<xsl:value-of select="substring($altezza, 0, string-length($altezza) - 1) - 20"/>
									</xsl:when>
									<xsl:otherwise>
										<xsl:value-of select="$altezza"/>
									</xsl:otherwise>
								</xsl:choose>
							</xsl:variable>
							<!--<xsl:message terminate="yes">height: <xsl:value-of select="$altezza"/>, um: <xsl:value-of select="$um-altezza"/>, real: <xsl:value-of select="$altezza-reale"/></xsl:message>-->
							<xsl:choose>
								<xsl:when test="position() = 1">display: block;</xsl:when>
								<xsl:otherwise>display: none;</xsl:otherwise>
							</xsl:choose>overflow: auto; height: <xsl:value-of select="$altezza-reale"/>; width: 100%; padding-right: 4px;
							<xsl:value-of select="@style"/>
						</xsl:attribute>
					
						<xsl:apply-templates/>
					
					</xsl:element>
				</xsl:for-each>
			</td>
			<td class="{concat($class, '_r')}"/>
		</tr>

		

		<!-- riga grafica finale -->
		<tr>
			<td class="{concat($class, '_bl')}"/>
			<td class="{concat($class, '_b')}" id="{concat('tabstripbottom_', $numero)}" colspan="{2 * $numero-pagine + 1}"/>
			<td class="{concat($class, '_br')}"/>
		</tr>

	</xsl:element>

	<!-- script di attivazione -->
	<xsl:if test="count(gfx:page/gfx:hide-if)">
		<script language="Javascript">
			<xsl:element name="ubk:when">
				<xsl:for-each select="gfx:page">
					<xsl:sort select="position()" order="descending" data-type="number" /> 

					<xsl:variable name="pos" select="last() - position()"/>
					<xsl:variable name="posit" select="position()"/>
					<xsl:variable name="lastid" select="last()"/>
					
					<xsl:if test="gfx:hide-if">
						<xsl:element name="ubk:test"><xsl:value-of select="gfx:hide-if/text()"/></xsl:element>
						<xsl:element name="ubk:then">
							tabstrip_deactivate( <xsl:value-of select="$pos"/>, '<xsl:value-of select="$numero"/>' );
						</xsl:element>
					</xsl:if>
				</xsl:for-each>
			</xsl:element>

			tabstrip_force( 0, '<xsl:value-of select="$numero"/>' );
			<!-- tabstrip_restore('<xsl:value-of select="$numero"/>', '<xsl:value-of select="@larghezza"/>'); --> 
		</script>
	</xsl:if>

		

</xsl:template>

</xsl:stylesheet>