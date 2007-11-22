<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<!-- 
	Forza il charset UTF-8 per pagine create senza la gfx:page
-->
<!--
<xsl:template match="head">
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
		<xsl:apply-templates/>
	</head>
</xsl:template>
-->
<!-- 
	Questa mi ritorna la copia di un nodo qualunque, e richiama la copia del contenuto, degli attributi e dei figli 
	Sulle td ciocchi per le classi della gfx:table
-->
<xsl:template match="*">
	
<!--	 <xsl:choose>
		<xsl:when test="name() = 'td'" > 
			<xsl:variable name="table_padre" select="ancestor::table[position() = 1]"/>
			<xsl:choose>
				<xsl:when test="$table_padre/@gfx = '1'">
					<xsl:copy>
						<xsl:attribute name="class">
							<xsl:value-of select="concat(@class, ' ', $table_padre/@class)"/>
						</xsl:attribute>						
						<xsl:apply-templates select="*|@*|text()"/>
					</xsl:copy>

				</xsl:when>
				<xsl:otherwise>
					<xsl:copy>
						<xsl:apply-templates select="*|@*|text()"/>
					</xsl:copy>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:when>

		<xsl:otherwise>-->
			<xsl:copy>
				<xsl:apply-templates select="*|@*|text()"/>
			</xsl:copy>
			<!--
		</xsl:otherwise>
	</xsl:choose> -->
</xsl:template>

<!-- Questa mi ritorna una copia del testo (contenuto) di un tag, o di un attributo -->
<xsl:template match="text()|@*">
	<xsl:copy/>
</xsl:template>

<!-- 
	Check per form annidati
-->
<xsl:template match="ubk:form|form|FORM|ubk:ajax-form">
	<xsl:choose>
		<xsl:when test="count(ancestor::*[name() = 'form' or name() = 'FORM' or name() = 'ubk:form' or name() = 'ubk:ajax-form'])">
			<xsl:message terminate="yes">Attenzione, form annidati sulla pagina.</xsl:message>
		</xsl:when>
		<xsl:otherwise>
			<xsl:element name="{name(.)}">
				<xsl:apply-templates select="*|@*|text()"/>
			</xsl:element>
		</xsl:otherwise>
	</xsl:choose>

</xsl:template>


<!-- 
	Debug
-->
<xsl:template name="node-path">
	<xsl:param name="node"/>
	<xsl:value-of select="name($node)"/><xsl:if test="count($node/ancestor::node()) &gt; 0">|<xsl:call-template name="node-path">
			<xsl:with-param name="node" select="$node/.."/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

</xsl:stylesheet>