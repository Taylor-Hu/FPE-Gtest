<?xml version="1.0" encoding="GB2312"?> 
<xsl:stylesheet version="1.0" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
  
<xsl:template match="/"> 
  <html> 
  <body> 
  <h2 align="center">长虹网络公司CHDRV测试报告(V0.02)</h2>  
  <xsl:apply-templates select="testsuites/testhead"/>  
  <xsl:apply-templates select="testsuites"/>  
  </body> 
  </html> 
</xsl:template> 
 
 <xsl:template match="testsuites/testhead"> 
    <table border="1" width="100%"> 
    <tr> 
      <td bgcolor="#9acd32">Project Name</td> 
      <td><xsl:value-of select="@name"/></td> 
    </tr> 
    <tr> 
      <td bgcolor="#9acd32">Build Path</td> 
      <td><xsl:value-of select="@path"/></td> 
    </tr> 
    <tr> 
      <td bgcolor="#9acd32">Build Date</td> 
      <td><xsl:value-of select="@date"/></td> 
    </tr> 
	<tr> 
      <td bgcolor="#9acd32">Tester</td> 
      <td><xsl:value-of select="@tester"/></td> 
    </tr>
     </table>
	  <br/> 
</xsl:template> 
 
<xsl:template match="testsuites"> 
    <table border="1" width="100%"> 
    <tr bgcolor="#9acd32"> 
      <th align="left">module</th> 
      <th align="left">tests</th> 
      <th align="left">time</th> 
      <th align="left">failed</th> 
    </tr> 
 
    	<xsl:choose> 
        <xsl:when test="@failures &gt; 0"> 
          <tr bgcolor="#ff0000"> 
          <td><xsl:value-of select="@name"/></td> 
		      <td><xsl:value-of select="@tests"/></td> 
		      <td><xsl:value-of select="@time"/></td> 
		      <td><xsl:value-of select="@failures"/></td>   
		      </tr> 
        </xsl:when> 
        <xsl:otherwise> 
          <tr> 
          <td><xsl:value-of select="@name"/></td> 
		      <td><xsl:value-of select="@tests"/></td> 
		      <td><xsl:value-of select="@time"/></td> 
		      <td><xsl:value-of select="@failures"/></td>     
		      </tr>            	 
        </xsl:otherwise> 
      </xsl:choose>       
 
    </table> 
    <br/> 
     
    <xsl:apply-templates select="testsuite"/> 
 
    <br/><p></p> 
    <h2 align="center">failed test case</h2> 
    <table border="1"  width="100%"> 
    <tr bgcolor="red"> 
    	<th align="left">module name</th> 
      <th align="left">case name</th> 
      <th align="left">description</th> 
      <th align="left">message</th> 
    </tr> 
      <xsl:apply-templates select="testsuite/testcase/failure"/> 
    </table> 
</xsl:template> 

<xsl:template match="testsuite"> 
    <h2><xsl:value-of select="@name"/></h2> 
    <table border="1"  width="100%"> 
    <tr bgcolor="#9acd32"> 
      <th align="left">module</th> 
      <th align="left">tests</th> 
      <th align="left">time</th> 
      <th align="left">failed</th> 
    </tr> 
    	<xsl:choose> 
        <xsl:when test="@failures &gt; 0"> 
          <tr bgcolor="#ff0000"> 
          <td><xsl:value-of select="@name"/></td> 
		      <td><xsl:value-of select="@tests"/></td> 
		      <td><xsl:value-of select="@time"/></td> 
		      <td><xsl:value-of select="@failures"/></td>   
		      </tr> 
        </xsl:when> 
        <xsl:otherwise> 
          <tr> 
          <td><xsl:value-of select="@name"/></td> 
		      <td><xsl:value-of select="@tests"/></td> 
		      <td><xsl:value-of select="@time"/></td> 
		      <td><xsl:value-of select="@failures"/></td>     
		      </tr>            	 
        </xsl:otherwise> 
      </xsl:choose>       
 
    </table> 
</xsl:template> 
 
 
<xsl:template match="testcase"> 
    <tr> 
      <td><xsl:value-of select="@name"/></td> 
      <td><xsl:value-of select="../@name"/></td> 
      <td><xsl:value-of select="failure/@message"/></td>    
    </tr> 
</xsl:template> 
 
<xsl:template match="failure"> 
    <tr> 
    	<td><xsl:value-of select="../../@name"/></td> 
      <td><xsl:value-of select="../@name"/></td> 
      <td><xsl:value-of select="../@description"/></td>      
	  <td><xsl:value-of select="." disable-output-escaping="yes"/></td> 
    </tr> 
</xsl:template> 
 
</xsl:stylesheet>
