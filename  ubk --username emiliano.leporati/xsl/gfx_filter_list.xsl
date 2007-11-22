<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<!-- ************************************************************** -->

<!--               T E M P L A T E                                  -->

<!-- ************************************************************** -->
<xsl:template match="gfx:filter-list">

	<xsl:if test="count(gfx:filter) != 1 or count(gfx:list) != 1 or count(gfx:filter/gfx:head) != 1 or count(gfx:filter/gfx:body) != 1 or count(gfx:list/gfx:head) != 1 or count(gfx:list/gfx:body) != 1">
		<xsl:message terminate="yes">Template 'gfx:filter-list' non utilizzato correttamente.</xsl:message>
	</xsl:if>

	<xsl:variable name="class">
		<xsl:choose>
			<xsl:when test="@class"><xsl:value-of select="@class"/></xsl:when>
			<xsl:otherwise>gas</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="evidenzia">
		<xsl:choose>
			<xsl:when test="@evidenzia"><xsl:value-of select="@evidenzia"/></xsl:when>
			<xsl:otherwise>false</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="align">
		<xsl:choose>
			<xsl:when test="@align"><xsl:value-of select="@align"/></xsl:when>
			<xsl:otherwise>left</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="prima-riga-filtro" select="gfx:filter/gfx:head/tr[position()=1]/*"/>
	<xsl:variable name="numero-colonne-filtro" select="count($prima-riga-filtro) + sum($prima-riga-filtro/@colspan) - count($prima-riga-filtro[@colspan])"/>

	<xsl:variable name="prima-riga-elenco" select="gfx:list/gfx:head/tr[position()=1]/*"/>
	<xsl:variable name="numero-colonne-elenco" select="count($prima-riga-elenco) + sum($prima-riga-elenco/@colspan) - count($prima-riga-elenco[@colspan])"/>

	<!-- tabella di filtro -->
	<div style="margin: 0; padding: 0; border: 0; display: block; text-align: {$align}">
		<xsl:call-template name="gfx-filter-list-filter">
			<xsl:with-param name="class" select="$class"/>
			<xsl:with-param name="larghezza">98%</xsl:with-param>
			<xsl:with-param name="numero-colonne" select="$numero-colonne-filtro"/>
		</xsl:call-template>
	</div>

	<!-- tabella di elenco -->

	<xsl:variable name="test">
		<xsl:choose>
			<xsl:when test="@test"><xsl:value-of select="@test"/></xsl:when>
			<xsl:otherwise>$parametri["GESTORE"]->eof</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="messaggio">
		<xsl:choose>
			<xsl:when test="@messaggio-no-dati"><xsl:value-of select="@messaggio-no-dati"/></xsl:when>
			<xsl:otherwise>Nessun dato rispondente ai criteri.</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	
	
	<xsl:element name="ubk:if">
		<xsl:element name="ubk:test"><xsl:value-of select="$test"/></xsl:element>
		<xsl:element name="ubk:then"><div style="margin-top: 10px"><xsl:value-of select="$messaggio"/></div></xsl:element>
		<xsl:element name="ubk:else">
			<div style="width: 100%; position: absolute; bottom: 0; margin: 0; padding: 0; border: 0; display: block; text-align: {$align}">
				<xsl:call-template name="gfx-filter-list-list">
					<xsl:with-param name="class" select="$class"/>
					<xsl:with-param name="larghezza">98%</xsl:with-param>
					<xsl:with-param name="numero-colonne" select="$numero-colonne-elenco"/>
					<xsl:with-param name="evidenzia" select="$evidenzia"/>
					<xsl:with-param name="stat" select="@stat"/>
				</xsl:call-template>
			</div>
		</xsl:element>
	</xsl:element>

 </xsl:template>



<!--  T A B E L L A   F I L T R O  -->
<xsl:template name="gfx-filter-list-filter">
	<xsl:param name="class"/>
	<xsl:param name="numero-colonne"/>
	<xsl:param name="larghezza"/>

	<!-- tabella -->
	<xsl:element name="table">
		<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
		<xsl:attribute name="style">width: <xsl:value-of select="$larghezza"/>;</xsl:attribute>
		<xsl:attribute name="gfx">1</xsl:attribute>


		<xsl:for-each select="gfx:filter/gfx:head">
			<xsl:call-template name="gfx-table-head-sort">
				<xsl:with-param name="class" select="$class"/>
				<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
			</xsl:call-template>
		</xsl:for-each>

		
		<!-- righe intestazione e body -->
		<xsl:for-each select="gfx:filter/gfx:head/tr">
			<tr>
				<td class="{concat($class, '_thl')}"/>
				<xsl:for-each select="td|th">
					<xsl:element name="th">
						<xsl:for-each select="@*[name() != 'class']">
							<xsl:copy/>
						</xsl:for-each>
						<xsl:attribute name="class">
							<xsl:value-of select="concat(@class, ' ', $class)"/>
						</xsl:attribute>

						<xsl:apply-templates/>

					</xsl:element>
				</xsl:for-each>
				<td class="{concat($class, '_thr')}"/>
			</tr>
		</xsl:for-each>

		<xsl:element name="ubk:form">
			<xsl:for-each select="gfx:filter/gfx:body/*">
				<xsl:call-template name="gfx-table-riga-complessa">
					<xsl:with-param name="alternata">false</xsl:with-param>
					<xsl:with-param name="class" select="$class"/>
					<xsl:with-param name="evidenzia">false</xsl:with-param>
				</xsl:call-template>
			</xsl:for-each>
		</xsl:element>

		<!-- riga di chiusura -->
		<tr style="height: 1px">
			<td class="{concat($class, '_bl')}"/>
			<td class="{concat($class, '_bc')}" colspan="{$numero-colonne}"/>
			<td class="{concat($class, '_br')}"/>
		</tr>

	</xsl:element>
</xsl:template>


<!--  T A B E L L A   E L E N C O  -->
<xsl:template name="gfx-filter-list-list">
	<xsl:param name="class"/>
	<xsl:param name="numero-colonne"/>
	<xsl:param name="larghezza"/>
	<xsl:param name="evidenzia"/>
	<xsl:param name="stat"/>

	<!-- tabella -->
	<xsl:element name="table">
		<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
		<xsl:attribute name="style">width: <xsl:value-of select="$larghezza"/>;</xsl:attribute>
		<xsl:attribute name="gfx">1</xsl:attribute>

		<!-- riga di apertura -->
		<xsl:for-each select="gfx:list/gfx:head">
			<xsl:call-template name="gfx-table-head-sort">
				<xsl:with-param name="class" select="$class"/>
				<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
			</xsl:call-template>
		</xsl:for-each>

		
		<!-- righe intestazione -->
		<xsl:for-each select="gfx:list/gfx:head/tr">
			<tr>
				<td class="{concat($class, '_thl')}"/>
				<xsl:for-each select="td|th">
					<xsl:element name="th">
						<xsl:for-each select="@*[name() != 'class']">
							<xsl:copy/>
						</xsl:for-each>
						<xsl:attribute name="class">
							<xsl:value-of select="concat(@class, ' ', $class)"/>
						</xsl:attribute>

						<xsl:apply-templates/>

					</xsl:element>
				</xsl:for-each>
				<td class="{concat($class, '_thr')}"/>
			</tr>
		</xsl:for-each>

		<!-- body -->
		<xsl:element name="ubk:form">

			<xsl:element name="ubk:righe">
				<xsl:for-each select="gfx:list/gfx:body/*">
					<xsl:call-template name="gfx-table-riga-complessa">
						<xsl:with-param name="alternata">true</xsl:with-param>
						<xsl:with-param name="class" select="$class"/>
						<xsl:with-param name="evidenzia" select="$evidenzia"/>
					</xsl:call-template>
				</xsl:for-each>
			</xsl:element>

			<!-- barra spostamento -->
			<xsl:call-template name="gfx-table-riga-spostamento">
				<xsl:with-param name="spostamento">true</xsl:with-param>
				<xsl:with-param name="class" select="$class"/>
				<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
			</xsl:call-template>

		</xsl:element>

		<!-- riga di chiusura -->
		<tr style="height: 1px">
			<td class="{concat($class, '_bl')}"/>
			<td class="{concat($class, '_bc')}" colspan="{$numero-colonne}"/>
			<td class="{concat($class, '_br')}"/>
		</tr>

	</xsl:element>

	<!-- statistiche -->
	<xsl:if test="$stat = 'true'">
		<xsl:call-template name="gfx-include">
			<xsl:with-param name="file">sgm_stat.xml</xsl:with-param>
		</xsl:call-template>
	</xsl:if>


</xsl:template>

</xsl:stylesheet>