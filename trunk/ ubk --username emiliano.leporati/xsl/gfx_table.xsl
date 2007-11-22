<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<!-- ************************************************************** -->

<!--               T E M P L A T E                                  -->

<!-- ************************************************************** -->

<xsl:template match="gfx:table">

	<xsl:if test="not(@class)">
		<xsl:message terminate="yes">Manca attributo "class" in tag "gfx:table".</xsl:message>
	</xsl:if>

	<!-- numero colonne -->

	<xsl:variable name="numero-colonne">
		<xsl:choose>
			<xsl:when test="@cols">
				<xsl:value-of select="@cols"/>
			</xsl:when>
			<xsl:when test="count(gfx:head[position()=1]/tr[position()=1]/*) &gt; 0">
				<xsl:variable name="prima-riga" select="gfx:head[position()=1]/tr[position()=1]/*"/>
				<xsl:value-of select="count($prima-riga[not(@colspan)]) + sum($prima-riga/@colspan)"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="prima-riga" select="gfx:body[position()=1]//tr[position() = 1]/*"/>
				<xsl:value-of select="count($prima-riga[not(@colspan)]) + sum($prima-riga/@colspan)"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>


	<!-- stampa -->

	<xsl:choose>
		<xsl:when test="@test-no-dati = 'true'">
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
					<xsl:call-template name="gfx-table">
						<xsl:with-param name="class" select="@class"/>
						<xsl:with-param name="style" select="@style"/>
						<xsl:with-param name="larghezza" select="@larghezza"/>
						<xsl:with-param name="altezza" select="@altezza"/>
						<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
						<xsl:with-param name="spostamento" select="@spostamento"/>
						<xsl:with-param name="stat" select="@stat"/>
					</xsl:call-template>
				</xsl:element>
			</xsl:element>
		</xsl:when>

		<xsl:otherwise>
			<xsl:call-template name="gfx-table">
				<xsl:with-param name="class" select="@class"/>
				<xsl:with-param name="style" select="@style"/>
				<xsl:with-param name="larghezza" select="@larghezza"/>
				<xsl:with-param name="altezza" select="@altezza"/>
				<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
				<xsl:with-param name="spostamento" select="@spostamento"/>
				<xsl:with-param name="stat" select="@stat"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>

</xsl:template>

<xsl:template match="gfx:table//tr[not(name(..) = 'table')]">
	<xsl:call-template name="gfx-table-riga-complessa">
		<xsl:with-param name="alternata" select="ancestor::gfx:table/@alternata"/>
		<xsl:with-param name="class" select="ancestor::gfx:table/@class"/>
		<xsl:with-param name="evidenzia" select="ancestor::gfx:table/@evidenzia"/>
	</xsl:call-template>
</xsl:template>



<!-- T A B E L L A -->

<!-- da chiamare con un gfx:table nel contesto -->

<xsl:template name="gfx-table">
	<xsl:param name="class"/>
	<xsl:param name="style"/>
	<xsl:param name="larghezza"/>
	<xsl:param name="altezza"/>
	<xsl:param name="numero-colonne"/>
	<xsl:param name="spostamento"/>
	<xsl:param name="stat"/>

	<!-- conto quante colonne ha -->

	<!-- tabella -->
	<xsl:element name="table">
		<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
		<xsl:attribute name="style"><xsl:if test="$larghezza">width: <xsl:value-of select="$larghezza"/>;</xsl:if><xsl:if test="$altezza">height: <xsl:value-of select="$altezza"/>;</xsl:if><xsl:if test="$style"><xsl:value-of select="$style"/></xsl:if></xsl:attribute>
		<xsl:attribute name="gfx">1</xsl:attribute>

		<!-- riga di apertura -->
		<xsl:call-template name="gfx-table-head-sort">
			<xsl:with-param name="class" select="@class"/>
			<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
		</xsl:call-template>

		
		<!-- righe intestazione e body -->
		<xsl:for-each select="gfx:head/tr|gfx:body/*">
			<xsl:choose>
				<xsl:when test="name(..) = 'gfx:head'">
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
				</xsl:when>
				<xsl:otherwise>
					<xsl:apply-templates select="."/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>

		<!-- barra spostamento -->
		<xsl:call-template name="gfx-table-riga-spostamento">
			<xsl:with-param name="spostamento" select="$spostamento"/>
			<xsl:with-param name="class" select="$class"/>
			<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
		</xsl:call-template>

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


<!-- R I G A   D I   A P E R T U R A -->

<!-- da chiamare con un gfx:head nel contesto -->
<xsl:template name="gfx-table-head-sort">
	<xsl:param name="$class"/>
	<xsl:param name="$numero-colonne"/>

	<xsl:choose>
		<xsl:when test="boolean(gfx:head/tr-sort)">
		miao
		<!-- riga con l'ordinamento panico -->
			<tr style="height: 1px">
				<td class="{concat($class, '_sort_l')}"/>

				<xsl:for-each select="gfx:head/tr-sort/td|gfx:head/tr-sort/th">
					<xsl:element name="td">
						<xsl:attribute name="class"><xsl:value-of select="concat($class, '_sort')"/></xsl:attribute>
						
						<xsl:for-each select="*">
							<xsl:choose>
								<xsl:when test="name() = 'ordina'">
									<xsl:call-template name="ordina">
										<xsl:with-param name="class" select="$class"/>
									</xsl:call-template>
								</xsl:when>
								<xsl:otherwise>
									<xsl:apply-templates/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:for-each>

					</xsl:element>
				</xsl:for-each>

				<td class="{concat($class, '_sort_r')}"/>
			</tr>
		</xsl:when>
		<xsl:otherwise>
			<!-- riga di apertura -->
			<tr style="height: 1px">
				<td class="{concat($class, '_tl')}"/>
				<td class="{concat($class, '_tc')}" colspan="{$numero-colonne}"/>
				<td class="{concat($class, '_tr')}"/>
			</tr>
		</xsl:otherwise>
	</xsl:choose>

</xsl:template>


<!-- R I G A    S P O S T A M E N T O -->
<xsl:template name="gfx-table-riga-spostamento">
	<xsl:param name="spostamento"/>
	<xsl:param name="class"/>
	<xsl:param name="numero-colonne"/>

	<xsl:if test="$spostamento = 'true'">
		<tr>
			<td class="{concat($class, '_l_move')}"/>
			<td class="{concat($class, '_c_move')}" colspan="{$numero-colonne}">

				<table style="height: 100%; width: 100%; border: 0; border-collapse: collapse; margin: 0; padding: 0; font-size: 0;">
					<td style="width: 10%; text-align: left; border; 0; padding: 0; margin: 0; background-color: transparent; white-space: nowrap;">
						<xsl:element name="ubk:link-primo">
							<xsl:element name="ubk:contenuto">
								<xsl:call-template name="gfx-img">
									<xsl:with-param name="nome">BBack</xsl:with-param>
								</xsl:call-template>
							</xsl:element>
						</xsl:element>
						<xsl:element name="ubk:link-precedente">
							<xsl:element name="ubk:contenuto">
								<xsl:call-template name="gfx-img">
									<xsl:with-param name="nome">Back</xsl:with-param>
								</xsl:call-template>
							</xsl:element>
						</xsl:element>
					</td>
					<td style="text-align: center; border; 0; padding: 0; margin: 0; background-color: transparent; white-space: nowrap;">
						<xsl:element name="ubk:paginatore" />
					</td>
					<td style="width: 10%; text-align: right; border; 0; padding: 0; margin: 0; background-color: transparent; white-space: nowrap;">
						<xsl:element name="ubk:link-successivo">
							<xsl:element name="ubk:contenuto">
								<xsl:call-template name="gfx-img">
									<xsl:with-param name="nome">Fore</xsl:with-param>
								</xsl:call-template>
							</xsl:element>
						</xsl:element>
						<xsl:element name="ubk:link-ultimo">
							<xsl:element name="ubk:contenuto">
								<xsl:call-template name="gfx-img">
									<xsl:with-param name="nome">FFore</xsl:with-param>
								</xsl:call-template>
							</xsl:element>
						</xsl:element>
					</td>
				</table>
			</td>
			<td class="{concat($class, '_r_move')}"/>
		</tr>
	</xsl:if>

</xsl:template>


<!-- R I G A    G E N E R I C A -->
<xsl:template name="gfx-table-riga-complessa">
	<xsl:param name="alternata"/>
	<xsl:param name="class"/>
	<xsl:param name="evidenzia"/>
	<xsl:param name="strip-last-cell"/>
	<xsl:param name="pos-last-cell"/>
	<xsl:choose>
		<xsl:when test="$evidenzia = 'true'">
			<xsl:choose>
				<xsl:when test="$alternata = 'true'">
					<xsl:element name="ubk:alt-tr">
						<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
						<xsl:attribute name="onmouseover">gfx_row_over(event, this)</xsl:attribute>
						<xsl:attribute name="onmouseout">gfx_row_out(event, this)</xsl:attribute>
						<xsl:apply-templates select="@*"/>
						<xsl:call-template name="gfx-table-riga"/>
					</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<tr onmouseover="gfx_row_over(event, this)" onmouseout="gfx_row_out(event, this)">
						<xsl:apply-templates select="@*"/>
						<xsl:call-template name="gfx-table-riga">
							<xsl:with-param name="class" select="$class"/>
							<xsl:with-param name="strip-last-cell" select="$strip-last-cell"/>
							<xsl:with-param name="pos-last-cell" select="$pos-last-cell"/>
						</xsl:call-template>
					</tr>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:when>
		<xsl:otherwise>
			<xsl:choose>
				<xsl:when test="$alternata = 'true'">
					<xsl:element name="ubk:alt-tr">
						<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
						<xsl:apply-templates select="@*"/>
						<xsl:call-template name="gfx-table-riga">
							<xsl:with-param name="class" select="$class"/>
							<xsl:with-param name="strip-last-cell" select="$strip-last-cell"/>
							<xsl:with-param name="pos-last-cell" select="$pos-last-cell"/>
						</xsl:call-template>
					</xsl:element>
				</xsl:when>
				<xsl:otherwise>
					<tr>
						<xsl:apply-templates select="@*"/>
						<xsl:call-template name="gfx-table-riga">
							<xsl:with-param name="class" select="$class"/>
							<xsl:with-param name="strip-last-cell" select="$strip-last-cell"/>
							<xsl:with-param name="pos-last-cell" select="$pos-last-cell"/>
						</xsl:call-template>
					</tr>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template name="gfx-table-riga">
	<xsl:param name="class"/>
	<xsl:param name="strip-last-cell"/>
	<xsl:param name="pos-last-cell"/>

	<td class="{concat($class, '_l')}"/>
	<xsl:for-each select="*[($strip-last-cell = 'true' and position() != $pos-last-cell) or not($strip-last-cell = 'true')]">
		<xsl:choose>
			<xsl:when test="name() = 'td' or name() = 'th'">
				<xsl:copy>
					<xsl:for-each select="@*[name() != 'class']">
						<xsl:copy/>
					</xsl:for-each>
					<xsl:attribute name="class">
						<xsl:value-of select="normalize-space(concat(@class, ' ', $class))"/>
					</xsl:attribute>

					<xsl:apply-templates/>
				</xsl:copy>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="."/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:for-each>
	<td class="{concat($class, '_r')}"/>
</xsl:template>

</xsl:stylesheet>