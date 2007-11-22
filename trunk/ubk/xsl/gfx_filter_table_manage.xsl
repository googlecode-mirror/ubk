<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">

<xsl:template match="gfx:filter-table-manage">

	<xsl:if test="count(gfx:filter) != 1 or count(gfx:filter/gfx:head) != 1 or count(gfx:filter/gfx:body) != 1 or count(gfx:list/gfx:head) != 1 or count(gfx:list/gfx:rw-body) != 1 or count(gfx:list/gfx:w-body) != 1">
		<xsl:message terminate="yes">Template 'gfx:filter-table-manage' non utilizzato correttamente.</xsl:message>
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

	<xsl:variable name="form-unico">
		<xsl:choose>
			<xsl:when test="@form-unico"><xsl:value-of select="@form-unico"/></xsl:when>
			<xsl:otherwise>true</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<xsl:variable name="prima-riga-filtro" select="gfx:filter/gfx:head/tr[position()=1]/*"/>
	<xsl:variable name="numero-colonne-filtro" select="count($prima-riga-filtro) + sum($prima-riga-filtro/@colspan) - count($prima-riga-filtro[@colspan])"/>

	<xsl:variable name="prima-riga-elenco" select="gfx:list/gfx:head/tr[position()=1]/*"/>
	<xsl:variable name="numero-colonne-elenco" select="count($prima-riga-elenco) + sum($prima-riga-elenco/@colspan) - count($prima-riga-elenco[@colspan])"/>

	<!-- diritto per la tabella in read-write -->
	<xsl:variable name="diritto-rw">
		<xsl:choose>
			<xsl:when test="gfx:list/gfx:rw-body/@diritto"><xsl:value-of select="gfx:list/gfx:rw-body/@diritto"/></xsl:when>
			<xsl:otherwise>RW</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<!-- diritto per la riga in write -->
	<xsl:variable name="diritto-w">
		<xsl:choose>
			<xsl:when test="gfx:list/gfx:w-body/@diritto"><xsl:value-of select="gfx:list/gfx:w-body/@diritto"/></xsl:when>
			<xsl:otherwise>W</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<!-- filtro -->
	<div style="margin: 0 0 5px 0; padding: 0; border: 0; display: block; text-align: {$align}">
		<xsl:call-template name="gfx-filter-list-filter">
			<xsl:with-param name="class" select="$class"/>
			<xsl:with-param name="larghezza">98%</xsl:with-param>
			<xsl:with-param name="numero-colonne" select="$numero-colonne-filtro"/>
		</xsl:call-template>
	</div>


	<!-- tabella rw con if x mancanza dati -->
	<xsl:choose>
		<xsl:when test="@test-no-dati = 'true'">
		true
			<xsl:variable name="test">
				<xsl:choose>
					<xsl:when test="@test"><xsl:value-of select="@test"/></xsl:when>
					<xsl:otherwise>$parametri["GESTORE"]->eof</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:variable name="messaggio">
				<xsl:choose>
					<xsl:when test="@messaggio-no-dati"><xsl:value-of select="@messaggio-no-dati"/></xsl:when>
					<xsl:otherwise>Nessun dato registrato.</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:element name="ubk:if">
				<xsl:element name="ubk:test"><xsl:value-of select="$test"/></xsl:element>
				<xsl:element name="ubk:then"><div><xsl:value-of select="$messaggio"/></div></xsl:element>
				<xsl:element name="ubk:else">
					<xsl:for-each select="gfx:list">
						<xsl:call-template name="gfx-table-manage-rw">
							<xsl:with-param name="class" select="$class"/>
							<xsl:with-param name="larghezza">98%</xsl:with-param>
							<xsl:with-param name="numero-colonne" select="$numero-colonne-elenco"/>
							<xsl:with-param name="evidenzia" select="$evidenzia"/>
							<xsl:with-param name="stat" select="@stat"/>
							<xsl:with-param name="diritto" select="$diritto-rw"/>
							<xsl:with-param name="form-unico" select="$form-unico"/>
							<xsl:with-param name="align" select="$align"/>
						</xsl:call-template>
					</xsl:for-each>
				</xsl:element>
			</xsl:element>
		</xsl:when>
		<xsl:otherwise>
			<xsl:for-each select="gfx:list">
				<xsl:call-template name="gfx-table-manage-rw">
					<xsl:with-param name="class" select="$class"/>
					<xsl:with-param name="larghezza">98%</xsl:with-param>
					<xsl:with-param name="numero-colonne" select="$numero-colonne-elenco"/>
					<xsl:with-param name="evidenzia" select="$evidenzia"/>
					<xsl:with-param name="stat" select="@stat"/>
					<xsl:with-param name="diritto" select="$diritto-rw"/>
					<xsl:with-param name="form-unico" select="$form-unico"/>
					<xsl:with-param name="align" select="$align"/>
				</xsl:call-template>
			</xsl:for-each>
		</xsl:otherwise>
	</xsl:choose>

	<!-- tabella w -->
	<xsl:for-each select="gfx:list">
		<xsl:call-template name="gfx-table-manage-w">
			<xsl:with-param name="class" select="$class"/>
			<xsl:with-param name="larghezza">98%</xsl:with-param>
			<xsl:with-param name="numero-colonne" select="$numero-colonne-elenco"/>
			<xsl:with-param name="diritto" select="$diritto-w"/>
			<xsl:with-param name="align" select="$align"/>
		</xsl:call-template>
	</xsl:for-each>

</xsl:template>


</xsl:stylesheet>