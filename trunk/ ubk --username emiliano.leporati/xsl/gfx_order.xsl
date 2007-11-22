<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template name="gfx:order">
	<xsl:param name="class" select="@class"/>

	<xsl:if test="not(@campo)">
		<xsl:message terminate="yes">Manca attributo "campo" in tag "gfx:order".</xsl:message>
	</xsl:if>

	<xsl:variable name="link">
		<table class="{concat($class, '_sort')}">
			<tr>
				<td class="{concat($class, '_sort_link')}">
					
					<xsl:element name="ubk:link-ordina">
						<xsl:attribute name="direzione">asc</xsl:attribute>
						<xsl:attribute name="class">sort</xsl:attribute>
						<xsl:attribute name="campo"><xsl:value-of select="@campo"/></xsl:attribute>
						<xsl:element name="ubk:contenuto">
							<xsl:call-template name="gfx-img">
								<xsl:with-param name="nome">sort/s_up</xsl:with-param>
								<xsl:with-param name="class">sort</xsl:with-param>
							</xsl:call-template>
						</xsl:element>
					</xsl:element>
					<xsl:element name="ubk:link-ordina">
						<xsl:attribute name="direzione">none</xsl:attribute>
						<xsl:attribute name="class">sort</xsl:attribute>
						<xsl:attribute name="campo"><xsl:value-of select="@campo"/></xsl:attribute>
						<xsl:element name="ubk:contenuto">
							<xsl:call-template name="gfx-img">
								<xsl:with-param name="nome">sort/s_clr</xsl:with-param>
								<xsl:with-param name="class">sort</xsl:with-param>
							</xsl:call-template>
						</xsl:element>
					</xsl:element>
					<xsl:element name="ubk:link-ordina">
						<xsl:attribute name="direzione">desc</xsl:attribute>
						<xsl:attribute name="class">sort</xsl:attribute>
						<xsl:attribute name="campo"><xsl:value-of select="@campo"/></xsl:attribute>
						<xsl:element name="ubk:contenuto">
							<xsl:call-template name="gfx-img">
								<xsl:with-param name="nome">sort/s_dwn</xsl:with-param>
								<xsl:with-param name="class">sort</xsl:with-param>
							</xsl:call-template>
						</xsl:element>
					</xsl:element>

				</td>
				
				<xsl:element name="ubk:if">
					<xsl:element name="ubk:test">
						$_file = str_replace("/","_",$parametri["FILE"]);
						return isSet($_SESSION["campi_ordine_$_file"]) and isSet($_SESSION["campi_ordine_$_file"]['<xsl:value-of select="@campo"/>']);
					</xsl:element>
					<xsl:element name="ubk:then">
						<xsl:element name="ubk:switch">
							<xsl:element name="ubk:test">
								$_file = str_replace("/","_",$parametri["FILE"]);
								$GLOBALS["pos"] = 1 + array_search('<xsl:value-of select="@campo"/>', array_keys($_SESSION["campi_ordine_$_file"]));
								return $_SESSION["campi_ordine_$_file"]['<xsl:value-of select="@campo"/>'];
							</xsl:element>
							<xsl:element name="ubk:case">"ASC"</xsl:element>
							<xsl:element name="ubk:then">
								<td class="{concat($class, '_sort_up')}">
									<xsl:element name="ubk:var">$GLOBALS["pos"]</xsl:element>
								</td>
							</xsl:element>
							<xsl:element name="ubk:case">"DESC"</xsl:element>
							<xsl:element name="ubk:then">
								<td class="{concat($class, '_sort_down')}">
									<xsl:element name="ubk:var">$GLOBALS["pos"]</xsl:element>
								</td>
							</xsl:element>
						</xsl:element>
					</xsl:element>
					<xsl:element name="ubk:else">
						<td class="{concat($class, '_sort_empty')}"/>
					</xsl:element>
				</xsl:element>
			</tr>
		</table>
	</xsl:variable>

	<xsl:apply-templates select="$link"/>

</xsl:template>

<xsl:template match="gfx:single-order">
	<xsl:if test="not(@campo)">
		<xsl:message terminate="yes">Manca attributo obbligatorio "campo" in tag "gfx:single-order".</xsl:message>
	</xsl:if>

	<xsl:call-template name="gfx-single-order">
		<xsl:with-param name="campo" select="@campo"/>
		<xsl:with-param name="class" select="@class"/>
	</xsl:call-template>

</xsl:template>

<xsl:template name="gfx-single-order">
	<xsl:param name="campo"/>
	<xsl:param name="class"/>

	<table style="border: 0; padding: 0; margin: 0">
		<tr>
			<td style="border: 0; padding: 0; margin: 0; vertical-align: middle;">
				<xsl:element name="ubk:php">
					$_file = str_replace("/","_",$parametri["FILE"]);
					if (isSet($_SESSION["campi_ordine_$_file"]['<xsl:value-of select="$campo"/>'])) {
						$GLOBALS['direzione_corrente'] = $_SESSION["campi_ordine_$_file"]['<xsl:value-of select="$campo"/>'];
						$GLOBALS['direzione'] = ($GLOBALS['direzione_corrente'] == "ASC") ? "DESC" : "ASC";
					} else {
						$GLOBALS['direzione'] = "ASC";
						$GLOBALS['direzione_corrente'] = "NONE";
					}
				</xsl:element>
				<xsl:element name="ubk:link-ordina">
					<xsl:attribute name="direzione">$GLOBALS['direzione']</xsl:attribute>
					<xsl:attribute name="campo"><xsl:value-of select="$campo"/></xsl:attribute>
					<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
					<xsl:attribute name="azione">ordina_singolo</xsl:attribute>
					<xsl:element name="ubk:contenuto"><xsl:apply-templates/></xsl:element>
				</xsl:element>
			</td>
			<td style="border: 0; padding: 0 0 0 2px; margin: 0; font-size: 0; vertical-align: middle;">
				<xsl:element name="ubk:switch">
					<xsl:element name="ubk:test">$GLOBALS['direzione_corrente']</xsl:element>
					<xsl:element name="ubk:case">"ASC"</xsl:element>
					<xsl:element name="ubk:then">
						<xsl:call-template name="gfx-img">
							<xsl:with-param name="nome">sort/s_up</xsl:with-param>
							<xsl:with-param name="class">sort</xsl:with-param>
						</xsl:call-template>
					</xsl:element>
					<xsl:element name="ubk:case">"DESC"</xsl:element>
					<xsl:element name="ubk:then">
						<xsl:call-template name="gfx-img">
							<xsl:with-param name="nome">sort/s_dwn</xsl:with-param>
							<xsl:with-param name="class">sort</xsl:with-param>
						</xsl:call-template>
					</xsl:element>
				</xsl:element>
			</td>
		</tr>
	</table>
</xsl:template>

</xsl:stylesheet>