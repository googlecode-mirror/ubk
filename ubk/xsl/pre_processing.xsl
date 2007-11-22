<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">

<xsl:output method="xml" encoding="UTF-8"/> 

<xsl:param name="phdir" select="'@PHDIR@/'"/>

<xsl:include href="xsl/gfx_include.xsl"/>


<!-- **************************************************************** -->
<!-- Procedure di definizione delle variabili utilizzate nei template -->
<!-- **************************************************************** -->


<xsl:template name="define-customs"> 
	<xsl:choose>
		<xsl:when test="@customs">
			<xsl:value-of select="concat($phdir, 'xsl/', @customs)"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="concat($phdir,'xsl/customs.xml')"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="define-appearence">
	<xsl:choose>
		<xsl:when test="@appearence">
			<xsl:value-of select="concat($phdir, 'xsl/', @appearence)"/>
		</xsl:when>
		<xsl:otherwise>
			<xsl:value-of select="concat($phdir,'xsl/appearence.xml')"/>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="define-type">
	<xsl:choose>
		<xsl:when test="@type"><xsl:value-of select="@type"/></xsl:when>
		<xsl:otherwise>view</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template name="define-visible">
	<xsl:param name="customs"/>

	<xsl:variable name="table" select="substring-before(@src, '.')"/>
	<xsl:variable name="field" select="substring-after(@src, '.')"/>

	<xsl:if test="not(document($customs)/customs/table[@name = $table]/field[@name = $field])">
		<xsl:message terminate="yes">Nessuna informazione di visualizzazione per <xsl:value-of select="concat($table, '.', $field)"/>.</xsl:message>
	</xsl:if>

	<xsl:value-of select="document($customs)/customs/table[@name = $table]/field[@name = $field]/@visible"/>
</xsl:template>

<xsl:template name="define-label">
	<xsl:param name="customs"/>

	<xsl:variable name="table" select="substring-before(@src, '.')"/>
	<xsl:variable name="field" select="substring-after(@src, '.')"/>

	<xsl:if test="not(document($customs)/customs/table[@name = $table]/field[@name = $field])">
		<xsl:message terminate="yes">Nessuna etichetta per <xsl:value-of select="concat($table, '.', $field)"/>.</xsl:message>
	</xsl:if>

	<xsl:value-of select="document($customs)/customs/table[@name = $table]/field[@name = $field]/@label"/>
</xsl:template>

<xsl:template name="define-view">
	<xsl:param name="appearence"/>
	<xsl:param name="type"/>

	<xsl:variable name="table" select="substring-before(@src, '.')"/>
	<xsl:variable name="field" select="substring-after(@src, '.')"/>

	<xsl:if test="not(document($appearence)/appearence/table[@name = $table]/field[@name = $field]/*[name() = $type])">
		<xsl:message terminate="yes">Nessuna visualizzazione di tipo <xsl:value-of select="$type"/> per <xsl:value-of select="concat($appearence, '/', $table, '.', $field)"/>.</xsl:message>
	</xsl:if>
	<xsl:variable name="field-content" select="document($appearence)/appearence/table[@name = $table]/field[@name = $field]/*[name() = $type]"/>
	<xsl:apply-templates select="$field-content/*|$field-content/text()"/>
</xsl:template>


<!-- ******************************************************** -->
<!-- Genera una cella di heading per una coppia tabella.campo -->
<!-- ******************************************************** -->
<xsl:template match="gfx:db-th">
	<xsl:if test="not(@src)">
		<xsl:message terminate="yes">Specificare l'attributo src per tag gfx:db-th.</xsl:message>
	</xsl:if>

	<xsl:variable name="customs"><xsl:call-template name="define-customs"/></xsl:variable>

	<xsl:variable name="visible">
		<xsl:call-template name="define-visible">
			<xsl:with-param name="customs" select="$customs"/>
		</xsl:call-template>
	</xsl:variable>
	<xsl:variable name="label">
		<xsl:call-template name="define-label">
			<xsl:with-param name="customs" select="$customs"/>
		</xsl:call-template>
	</xsl:variable>


	<xsl:if test="$visible = 1">
		<xsl:choose>
			<xsl:when test="@sort = 'true'">
				<xsl:variable name="field" select="substring-after(@src, '.')"/>
				<xsl:variable name="content">
					<xsl:element name="gfx:single-order">
						<xsl:attribute name="class">order</xsl:attribute>
						<xsl:attribute name="campo"><xsl:value-of select="$field"/></xsl:attribute>
						<xsl:value-of select="$label"/>
					</xsl:element>
				</xsl:variable>

				<xsl:choose>
					<xsl:when test="@class and @align">
						<th class="{@class}" style="text-align: {@align}">
							<xsl:apply-templates select="$content"/>
						</th>
					</xsl:when>
					<xsl:when test="@class">
						<th class="{@class}">
							<xsl:apply-templates select="$content"/>
						</th>
					</xsl:when>
					<xsl:when test="@align">
						<th style="text-align: {@align}">
							<xsl:apply-templates select="$content"/>
						</th>
					</xsl:when>
					<xsl:otherwise>
						<th>
							<xsl:apply-templates select="$content"/>
						</th>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="@class and @align">
						<th class="{@class}" style="text-align: {@align}">
							<xsl:apply-templates select="$label"/>
						</th>
					</xsl:when>
					<xsl:when test="@class">
						<th class="{@class}">
							<xsl:apply-templates select="$label"/>
						</th>
					</xsl:when>
					<xsl:when test="@align">
						<th style="text-align: {@align}">
							<xsl:apply-templates select="$label"/>
						</th>
					</xsl:when>
					<xsl:otherwise>
						<th>
							<xsl:apply-templates select="$label"/>
						</th>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:if>
</xsl:template>

<!-- *************************************************************** -->
<!-- Genera una cella con il campo html per una coppia tabella.campo -->
<!-- *************************************************************** -->
<xsl:template match="gfx:db-td">
	<xsl:if test="not(@src)">
		<xsl:message terminate="yes">Specificare l'attributo src per tag gfx:db-td.</xsl:message>
	</xsl:if>

	<xsl:variable name="customs"><xsl:call-template name="define-customs"/></xsl:variable>
	<xsl:variable name="appearence"><xsl:call-template name="define-appearence"/></xsl:variable>
	<xsl:variable name="type"><xsl:call-template name="define-type"/></xsl:variable>

	<xsl:variable name="visible">
		<xsl:call-template name="define-visible">
			<xsl:with-param name="customs" select="$customs"/>
		</xsl:call-template>
	</xsl:variable>

	<xsl:if test="$visible = 1">
		<xsl:choose>
			<xsl:when test="@without-td = 'true'">
				<xsl:call-template name="define-view">
					<xsl:with-param name="appearence" select="$appearence"/>
					<xsl:with-param name="type" select="$type"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="@class and @align">
						<td class="{@class}" style="text-align: {@align}">
							<xsl:call-template name="define-view">
								<xsl:with-param name="appearence" select="$appearence"/>
								<xsl:with-param name="type" select="$type"/>
							</xsl:call-template>
						</td>
					</xsl:when>
					<xsl:when test="@class">
						<td class="{@class}">
							<xsl:call-template name="define-view">
								<xsl:with-param name="appearence" select="$appearence"/>
								<xsl:with-param name="type" select="$type"/>
							</xsl:call-template>
						</td>
					</xsl:when>
					<xsl:when test="@align">
						<td style="text-align: {@align}">
							<xsl:call-template name="define-view">
								<xsl:with-param name="appearence" select="$appearence"/>
								<xsl:with-param name="type" select="$type"/>
							</xsl:call-template>
						</td>
					</xsl:when>
					<xsl:otherwise>
						<td>
							<xsl:call-template name="define-view">
								<xsl:with-param name="appearence" select="$appearence"/>
								<xsl:with-param name="type" select="$type"/>
							</xsl:call-template>
						</td>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:if>

</xsl:template>

<!-- ********************************************************* -->
<!-- Genera una riga di dettaglio per una coppia tabella.campo -->
<!-- ********************************************************* -->
<xsl:template match="gfx:db-row">
	<xsl:if test="not(@src)">
		<xsl:message terminate="yes">Specificare l'attributo src per tag gfx:db-row.</xsl:message>
	</xsl:if>

	<xsl:variable name="customs"><xsl:call-template name="define-customs"/></xsl:variable>
	<xsl:variable name="appearence"><xsl:call-template name="define-appearence"/></xsl:variable>
	<xsl:variable name="type"><xsl:call-template name="define-type"/></xsl:variable>

	<xsl:variable name="visible">
		<xsl:call-template name="define-visible">
			<xsl:with-param name="customs" select="$customs"/>
		</xsl:call-template>
	</xsl:variable>

	<xsl:variable name="label">
		<xsl:call-template name="define-label">
			<xsl:with-param name="customs" select="$customs"/>
		</xsl:call-template>
	</xsl:variable>


	<xsl:if test="$visible = 1">
		<xsl:choose>
			<xsl:when test="@without-tr = 'true'">
				<td class="label">
					<xsl:value-of select="$label"/>
				</td>
				<td>
					<xsl:call-template name="define-view">
						<xsl:with-param name="appearence" select="$appearence"/>
						<xsl:with-param name="type" select="$type"/>
					</xsl:call-template>
				</td>
			</xsl:when>
			<xsl:otherwise>
				<tr>
					<td class="label">
						<xsl:value-of select="$label"/>
					</td>
					<td>
						<xsl:call-template name="define-view">
							<xsl:with-param name="appearence" select="$appearence"/>
							<xsl:with-param name="type" select="$type"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:if>

</xsl:template>
<!-- ***************************************************************** -->
<!-- Genera l'etichetta memorizzata sul db di una coppia tabella.campo -->
<!-- ***************************************************************** -->
<xsl:template match="gfx:db-label">
	<xsl:if test="not(@src)">
		<xsl:message terminate="yes">Specificare l'attributo src per tag gfx:db-label.</xsl:message>
	</xsl:if>

	<xsl:variable name="customs"><xsl:call-template name="define-customs"/></xsl:variable>

	<xsl:call-template name="define-label">
		<xsl:with-param name="customs" select="$customs"/>
	</xsl:call-template>

</xsl:template>

<!-- *********************************************************************************************** -->
<!-- Visualizza il contenuto solo se il flag b_visibile sul db e' a uno per una coppia tabella.campo -->
<!-- *********************************************************************************************** -->
<xsl:template match="gfx:db-visible">
	<xsl:if test="not(@src)">
		<xsl:message terminate="yes">Specificare l'attributo src per tag gfx:db-visible.</xsl:message>
	</xsl:if>

	<xsl:variable name="table" select="substring-before(@src, '.')"/>
	<xsl:variable name="field" select="substring-after(@src, '.')"/>

	<xsl:variable name="customs"><xsl:call-template name="define-customs"/></xsl:variable>

	<xsl:variable name="visible">
		<xsl:call-template name="define-visible">
			<xsl:with-param name="customs" select="$customs"/>
		</xsl:call-template>
	</xsl:variable>


	<xsl:if test="$visible = 1">
		<xsl:apply-templates select="*"/>
	</xsl:if>

</xsl:template>


<!-- *********************************************************************************************** -->
<!-- Copia di tutto il resto per la post - elaborazione -->
<!-- *********************************************************************************************** -->


<!-- Questa mi ritorna la copia di un nodo qualunque, e richiama la copia del contenuto, degli attributi e dei figli -->
<xsl:template match="*">
	<xsl:copy>
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:copy>
</xsl:template>

<!-- Questa mi ritorna una copia del testo (contenuto) di un tag, o di un attributo -->
<xsl:template match="text()|@*">
	<xsl:copy/>
</xsl:template>

</xsl:stylesheet>