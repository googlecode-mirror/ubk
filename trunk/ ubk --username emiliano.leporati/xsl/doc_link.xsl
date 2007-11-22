<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<xsl:template match="ubk:link-crea">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-crea">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/crea']"/>
		</div>
	</xsl:if>

</xsl:template>


<xsl:template match="ubk:link-aggiorna">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-aggiorna">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/aggiorna']"/>
		</div>
	</xsl:if>

</xsl:template>

<xsl:template match="ubk:link-elimina">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-elimina">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/elimina']"/>
		</div>
	</xsl:if>

</xsl:template>

<xsl:template match="ubk:link-successivo">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-successivo">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/avanti']"/>
		</div>
	</xsl:if>

</xsl:template>

<xsl:template match="ubk:link-ultimo">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-ultimo">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/ultima']"/>
		</div>
	</xsl:if>
	
</xsl:template>

<xsl:template match="ubk:link-precedente">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-precedente">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/indietro']"/>
		</div>
	</xsl:if>

</xsl:template>

<xsl:template match="ubk:link-primo">

	<xsl:variable name="doc" select="concat($phdir, 'xsl/doc.xml')"/>

	<xsl:element name="ubk:link-primo">
		<xsl:apply-templates select="*|@*|text()"/>
	</xsl:element>

	<xsl:if test="@help-inline = 'true'">
		<div class="doc_div">
			<xsl:value-of select="document($doc)/doc/doc[@path = 'bottoni/prima']"/>
		</div>
	</xsl:if>

</xsl:template>


</xsl:stylesheet>