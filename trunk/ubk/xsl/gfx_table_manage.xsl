<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gfx="http://dev.mele.eu/ubk/TAG_XSL.xsd"
		xmlns:ubk="http://dev.mele.eu/ubk/"
		extension-element-prefixes="gfx ubk">


<!-- ************************************************************** -->

<!--               T E M P L A T E                                  -->

<!-- ************************************************************** -->

<xsl:template match="gfx:table-manage">

	<xsl:if test="count(gfx:head) != 1 or count(gfx:rw-body) != 1 or count(gfx:w-body) != 1">
		<xsl:message terminate="yes">Template 'gfx:table-manage' non utilizzato correttamente.</xsl:message>
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

	<xsl:variable name="prima-riga" select="gfx:head/tr[position()=1]/*"/>
	<xsl:variable name="numero-colonne" select="count($prima-riga) + sum($prima-riga/@colspan) - count($prima-riga[@colspan])"/>


	<!-- diritto per la tabella in read-write -->
	<xsl:variable name="diritto-rw">
		<xsl:choose>
			<xsl:when test="gfx:rw-body/@diritto"><xsl:value-of select="gfx:rw-body/@diritto"/></xsl:when>
			<xsl:otherwise>RW</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>

	<!-- diritto per la riga in write -->
	<xsl:variable name="diritto-w">
		<xsl:choose>
			<xsl:when test="gfx:w-body/@diritto"><xsl:value-of select="gfx:w-body/@diritto"/></xsl:when>
			<xsl:otherwise>W</xsl:otherwise>
		</xsl:choose>
	</xsl:variable>


	<!-- tabella rw con if x mancanza dati -->
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
					<xsl:otherwise>Nessun dato registrato.</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:element name="ubk:if">
				<xsl:element name="ubk:test"><xsl:value-of select="$test"/></xsl:element>
				<xsl:element name="ubk:then"><div><xsl:value-of select="$messaggio"/></div></xsl:element>
				<xsl:element name="ubk:else">
					<xsl:call-template name="gfx-table-manage-rw">
						<xsl:with-param name="class" select="$class"/>
						<xsl:with-param name="larghezza">98%</xsl:with-param>
						<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
						<xsl:with-param name="evidenzia" select="$evidenzia"/>
						<xsl:with-param name="stat" select="@stat"/>
						<xsl:with-param name="diritto" select="$diritto-rw"/>
						<xsl:with-param name="form-unico" select="$form-unico"/>
						<xsl:with-param name="align" select="$align"/>
					</xsl:call-template>
				</xsl:element>
			</xsl:element>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="gfx-table-manage-rw">
				<xsl:with-param name="class" select="$class"/>
				<xsl:with-param name="larghezza">98%</xsl:with-param>
				<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
				<xsl:with-param name="evidenzia" select="$evidenzia"/>
				<xsl:with-param name="stat" select="@stat"/>
				<xsl:with-param name="diritto" select="$diritto-rw"/>
				<xsl:with-param name="form-unico" select="$form-unico"/>
				<xsl:with-param name="align" select="$align"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>

	<!-- tabella w -->
	<xsl:call-template name="gfx-table-manage-w">
		<xsl:with-param name="class" select="$class"/>
		<xsl:with-param name="larghezza">98%</xsl:with-param>
		<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
		<xsl:with-param name="diritto" select="$diritto-w"/>
		<xsl:with-param name="align" select="$align"/>
	</xsl:call-template>


 </xsl:template>





<xsl:template name="gfx-table-manage-rw">
	<xsl:param name="class"/>
	<xsl:param name="numero-colonne"/>
	<xsl:param name="larghezza"/>
	<xsl:param name="evidenzia"/>
	<xsl:param name="stat"/>
	<xsl:param name="diritto"/>
	<xsl:param name="form-unico"/>
	<xsl:param name="align"/>


	<xsl:element name="ubk:usa">
		<xsl:attribute name="mode">RW</xsl:attribute>
		<xsl:element name="ubk:sezione">
			<xsl:attribute name="diritto"><xsl:value-of select="$diritto"/></xsl:attribute>
			<xsl:attribute name="fallisci">false</xsl:attribute>
			<xsl:attribute name="restringi">false</xsl:attribute>

			<div style="margin: 0; padding: 0; border: 0; display: block; text-align: {$align}">

				<!-- tabella -->
				<xsl:element name="table">
					<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
					<xsl:attribute name="style">width: <xsl:value-of select="$larghezza"/>;</xsl:attribute>
					<xsl:attribute name="gfx">1</xsl:attribute>

					<!-- riga apertura -->
					<xsl:for-each select="gfx:head">
						<xsl:call-template name="gfx-table-head-sort">
							<xsl:with-param name="class" select="$class"/>
							<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
						</xsl:call-template>
					</xsl:for-each>

					
					<!-- righe intestazione -->
					<xsl:for-each select="gfx:head/tr">
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

					<!-- righe body -->
					<xsl:choose>
						<xsl:when test="$form-unico = 'false'">
							<!-- body -->
							<xsl:element name="ubk:righe">
								<xsl:element name="ubk:form">
									<xsl:for-each select="gfx:rw-body/*">
										<xsl:call-template name="gfx-table-riga-complessa">
											<xsl:with-param name="alternata">true</xsl:with-param>
											<xsl:with-param name="class" select="$class"/>
											<xsl:with-param name="evidenzia" select="$evidenzia"/>
										</xsl:call-template>
									</xsl:for-each>
								</xsl:element>
							</xsl:element>
							<!-- spostamento -->
							<xsl:element name="ubk:form">
								<xsl:call-template name="gfx-table-riga-spostamento">
									<xsl:with-param name="spostamento">true</xsl:with-param>
									<xsl:with-param name="class" select="$class"/>
									<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
								</xsl:call-template>
							</xsl:element>
						</xsl:when>

						<xsl:otherwise>
							<xsl:element name="ubk:form">
								<xsl:element name="ubk:righe">
									<!-- body -->
									<xsl:for-each select="gfx:rw-body/*">
										<xsl:call-template name="gfx-table-riga-complessa">
											<xsl:with-param name="alternata">true</xsl:with-param>
											<xsl:with-param name="class" select="$class"/>
											<xsl:with-param name="evidenzia" select="$evidenzia"/>
										</xsl:call-template>
									</xsl:for-each>
								</xsl:element>
								<!-- spostamento -->
								<xsl:call-template name="gfx-table-riga-spostamento">
									<xsl:with-param name="spostamento">true</xsl:with-param>
									<xsl:with-param name="class" select="$class"/>
									<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
								</xsl:call-template>
							</xsl:element>
						</xsl:otherwise>
					</xsl:choose>

					<!-- riga di chiusura -->
					<tr style="height: 1px">
						<td class="{concat($class, '_bl')}"/>
						<td class="{concat($class, '_bc')}" colspan="{$numero-colonne}"/>
						<td class="{concat($class, '_br')}"/>
					</tr>

				</xsl:element><!-- /table -->

				<!-- statistiche -->
				<xsl:if test="$stat = 'true'">
					<xsl:call-template name="gfx-include">
						<xsl:with-param name="file">sgm_stat.xml</xsl:with-param>
					</xsl:call-template>
				</xsl:if>
			
			</div>
		
		</xsl:element><!-- sezione -->
	</xsl:element><!-- usa -->

</xsl:template>



<xsl:template name="gfx-table-manage-w">
	<xsl:param name="class"/>
	<xsl:param name="numero-colonne"/>
	<xsl:param name="larghezza"/>
	<xsl:param name="diritto"/>
	<xsl:param name="align"/>

	<xsl:element name="ubk:usa">
		<xsl:attribute name="mode">W</xsl:attribute>
		<xsl:element name="ubk:sezione">
			<xsl:attribute name="diritto"><xsl:value-of select="$diritto"/></xsl:attribute>
			<xsl:attribute name="fallisci">false</xsl:attribute>

			<div style="position: absolute; bottom: 0; width: 100%; margin: 0; padding: 0; border: 0; display: block; text-align: {$align}">

				<!-- tabella -->
				<xsl:element name="table">
					<xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute>
					<xsl:attribute name="style">width: <xsl:value-of select="$larghezza"/>;</xsl:attribute>
					<xsl:attribute name="gfx">1</xsl:attribute>

					<!-- riga apertura -->
					<xsl:for-each select="gfx:head">
						<xsl:call-template name="gfx-table-head-sort">
							<xsl:with-param name="class" select="$class"/>
							<xsl:with-param name="numero-colonne" select="$numero-colonne"/>
						</xsl:call-template>
					</xsl:for-each>

					<!-- righe intestazione -->
					<xsl:for-each select="gfx:head/tr">
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
						<xsl:for-each select="gfx:w-body/*">
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

			</div>
		</xsl:element>
	</xsl:element>

</xsl:template>







</xsl:stylesheet>