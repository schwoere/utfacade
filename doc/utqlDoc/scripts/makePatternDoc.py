import glob
import sys
import os

_htmlFrame = """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<title>%(title)s</title>
<!-- <link rel="stylesheet" type="text/css" href="http://www.w3.org/StyleSheets/Core/Oldstyle"> -->
<style>
html body { font-family:"Lucida Grande", verdana, lucida, helvetica, sans-serif; font-size:8pt; }
td { padding:0.2em 0.7em 0.2em 0.7em; }
th { padding:0.2em 0.7em 0.2em 0.7em; background-color:#2c69ca; color:#f8f8f8; }
tr.tableCol1 { background-color:#fcfcfc; }
tr.tableCol2 { background-color:#f7f7f7; }
</style>
</head>
<body>
<h1>%(title)s</h1>
%(body)s
</body>
</html>
"""

_htmlPatternImage = "<p style=\"text-align:center\"><img src=\"%(location)s\"/></p><br/>\n"
_htmlNodeEdgeGroupHeader = "<h2>%s</h2>\n"
_htmlNodeEdgeHeader = "<h4>%s</h4>\n"
_htmlAttributeHeader = "<table>\n<tr><th>Attribute name</th><th>Value</th><th>Default</th><th>Description</th></tr>\n%(rows)s</table>\n"
_htmlAttributeRow = "<tr class=\"tableCol%(oddEven)d\"><td>%(name)s</td><td>%(value)s</td><td>%(default)s</td><td>%(description)s</td><tr>\n"
_htmlPredicate = "<p><b>Predicate: </b><code>%s</code></p>"
_htmlTocLine = "<a href=\"%(link)s\">%(title)s</a><br/>\n"
_htmlTocHeader = "<h%(level)d>%(name)s Patterns</h%(level)d>\n"
_htmlNewLine = "<br/>"
_htmlTable = "<table>\n%s</table>\n"
_htmlTableHeader = "<tr>%s</tr>\n"
_htmlTableHeaderElement = "<th>%s</th>"
_htmlTableRow = "<tr class=\"tableCol%(oddEven)d\">%(data)s</tr>\n"
_htmlTableRowElement = "<td>%s</td>"

# test for graphviz (does not seem to work...)
_haveGraphviz = True
try:
	_fh = os.popen( "dot", "w" )
	_fh.write( "blah" )
	_fh.close()
except IOError:
	_haveGraphviz = False

def _makePatternDocumentation( node ):
	"creates HTML documentation from a pattern dom node"
	
	doc = ""
	
	# add image
	if _haveGraphviz:
		doc += _htmlPatternImage % { 'location': node.getAttribute( "name" ) + ".png" }
	
	# find Description node
	doc += _getDescriptionHtml( node )
	
	# create documentation for nodes
	nodeDict = _makePatternObjectDict( node, "Node" )
	nodeDocContentLen = 0
	inputNodeDoc = ""
	outputNodeDoc = ""
	
	for n in nodeDict.itervalues():
		nodeNameDoc = _htmlNodeEdgeHeader % ( _prettyNodeName( n ) )
		nodeDocContent = _getDescriptionHtml( n ) 
		if n.parentNode.nodeName == "Input":
			nodeDocContent += _makePredicateDoc( n )
			nodeDocContent += _makeAttributeTable( n )
			inputNodeDoc += nodeNameDoc + nodeDocContent
		else:
			nodeDocContent += _makeAttributeTable( n )
			outputNodeDoc += nodeNameDoc + nodeDocContent
		nodeDocContentLen += len( nodeDocContent )

	# skip node empty documentation if we have graphviz and there is no extra documentation on the nodes, except what's already in the generated picture
	if not _haveGraphviz or nodeDocContentLen > 0:
		if inputNodeDoc != "":
			doc += ( _htmlNodeEdgeGroupHeader % ( "Input Nodes" ) ) + inputNodeDoc
		if outputNodeDoc != "":
			doc += ( _htmlNodeEdgeGroupHeader % ( "Output Nodes" ) ) + outputNodeDoc
	
	# create documentation for edges
	edgeDict = _makePatternObjectDict( node, "Edge" )
	inputEdgeDoc = ""
	outputEdgeDoc = ""
	
	for n in edgeDict.itervalues():
		nodeDoc = _htmlNodeEdgeHeader % ( _prettyNodeName( n ) )
		nodeDoc += _getDescriptionHtml( n ) 
		if n.parentNode.nodeName == "Input":
			nodeDoc += _makePredicateDoc( n )
			inputEdgeDoc += nodeDoc
		else:
			nodeDoc += _makeAttributeTable( n )
			outputEdgeDoc += nodeDoc
	
	if inputEdgeDoc != "":
		doc += ( _htmlNodeEdgeGroupHeader % ( "Input Edges" ) ) + inputEdgeDoc
	if outputEdgeDoc != "":
		doc += ( _htmlNodeEdgeGroupHeader % ( "Output Edges" ) ) + outputEdgeDoc
		
	# create documentation for data flow attributes
	dfDoc = ""
	for n in node.getElementsByTagName( "DataflowConfiguration" ):
		dfDoc += _makeAttributeTable( n )
		
	if len( dfDoc ) > 0:
		doc += ( _htmlNodeEdgeGroupHeader % ( "Dataflow Attributes" ) ) + dfDoc
	
	tgDoc = _makeTriggerGroupsDoc( node )
	if len( tgDoc ) > 0:
		doc += ( _htmlNodeEdgeGroupHeader % ( "Push/Pull Configurations" ) ) + tgDoc
	
	return doc

def _getDescriptionHtml( node ):
	"Creates an HTML representation of a UTQL element, based on its <Description>"
	
	doc = ""
	for child in node.childNodes:
		if child.nodeName == "Description":
			for desc in child.childNodes:
				doc += desc.toxml()
	if len( doc ) > 0:
		doc += "\n"
	
	# dirty hack to remove html namespace
	doc = doc.replace( "<h:", "<" ).replace( "</h:", "</" )
	return doc
	
def _makeAttributeTable( node ):
	"creates a table with documentation for all attributes"
	
	doc = ""
	i = 0
	for child in node.childNodes:
		if child.nodeName == "Attribute":
			i += 1
			doc += _htmlAttributeRow % { 'oddEven': (i%2)+1, 'name': child.getAttribute( "name" ), 'value': child.getAttribute( "value" ), \
				'default': child.getAttribute( "default" ), 'description': _getDescriptionHtml( child ) }
		
	if doc != "":
		doc = _htmlAttributeHeader % { 'rows': doc }
	return doc
	
def _makePredicateDoc( node ):
	"creates documentation for an input node/edge"
	
	doc = ""
	for child in node.childNodes:
		if child.nodeName == "Predicate":
			if child.firstChild != None:
				doc += _htmlPredicate % ( child.firstChild.toxml() )
		
	return doc
	
def _prettyNodeName( node, nameFirst = True, separator = ": " ):
	"returns the name of a node consisting of displayName and name attributes"
	
	name = node.getAttribute( "name" )
	if node.hasAttribute( "displayName" ):
		displayName = node.getAttribute( "displayName" )
		if name.replace( " ", "" ).lower() != displayName.replace( " ", "" ).lower(): # treat e.g. "CameraNode" as "camera node"
			if nameFirst:
				return name + separator + displayName
			else:
				return "%s [%s]" % ( displayName, name )
		else:
			return name
	else:
		return name
	
def _makePatternImage( pattern, fileName ):
	"creates a graphical view of the pattern in .png format. Needs graphviz to be installed and in the path."
	
	print "Creating image: " + fileName

	nodeDict = _makePatternObjectDict( pattern, "Node" )
	edgeDict = _makePatternObjectDict( pattern, "Edge" )
	
	# only patterns with less than 4 nodes are layouted from left to right
	if len( nodeDict ) < 4:
		rankdir = "LR"
	else:
		rankdir = "TB"
		
	graph = "digraph G { rankdir=%s; dpi=74;\n" % ( rankdir )
	
	# run over all nodes
	for node in nodeDict.itervalues():
		if node.parentNode.nodeName == "Output":
			style = "solid"
		else:
			style = "filled"
			
		graph += "node [label=\"%s\" style=%s fontname=Helvetica] %s;\n" % ( _prettyNodeName( node, True, "\\n" ), style, node.getAttribute( "name" ) )
	
	# run over all edges
	for edge in edgeDict.itervalues():
		if edge.parentNode.nodeName == "Output":
			style = "solid"
		else:
			style = "dashed"
		graph += "%s -> %s [label=\"%s\" style=%s fontname=Helvetica fontsize=12];\n" % ( edge.getAttribute( "source" ), edge.getAttribute( "destination" ), _prettyNodeName( edge ), style )
		
	graph += "}\n"
	
	# run graphviz
	graphviz = os.popen( "dot -Tpng -o \"%s\"" % ( fileName ), "w" )
	graphviz.write( graph )
	graphviz.close()

def _makeTriggerGroupsDoc( pattern ):
	"creates documentation for trigger groups (push/pull)"
	
	edgeDict = _makePatternObjectDict( pattern, "Edge" )
	
	doc = ""
	for group in pattern.getElementsByTagName( "TriggerGroup" ):
		# collect input and output edges
		inputs = []
		outputs = []
		for edge in group.getElementsByTagName( "Edge" ):
			ref = edge.getAttribute( "edge-ref" )
			orig = edgeDict[ ref ]
			if orig.parentNode.nodeName == "Output":
				outputs.append( ref )
			else:
				inputs.append( ref )
		
		# create table header
		header = ""
		for edgeRef in inputs:
			header += _htmlTableHeaderElement % ( edgeRef )
		for edgeRef in outputs:
			header += _htmlTableHeaderElement % ( edgeRef )
		header += _htmlTableHeaderElement % ( "condition" )
		table = _htmlTableHeader % ( header )
		
		# create table rows
		rowNum = 0
		
		# pull-pull only allowed if trigger group contains output
		stateStart = 0
		if len( outputs ) == 0:
			stateStart = 1
			
		for state in range( stateStart, 1 << len( inputs ) ):
			row = ""
			i = 0
			pushInputs = []
			for edgeRef in inputs:
				if state & (1 << i):
					row += _htmlTableRowElement % ( "push" )
					pushInputs.append( edgeRef )
				else:
					row += _htmlTableRowElement % ( "pull" )
				i += 1
				
			for edgeRef in outputs:
				if len( pushInputs ) > 0:
					row += _htmlTableRowElement % ( "push" )
				else:
					row += _htmlTableRowElement % ( "pull" )
			
			cond = ""
			if len( pushInputs ) > 1:
				cond = ", ".join( pushInputs ) + " must be synchronized"
				
			row += _htmlTableRowElement % ( cond )
			
			table += _htmlTableRow % { 'oddEven': ( rowNum % 1 ) + 1, 'data' : row }
			rowNum += 1
		
		doc += _htmlTable % ( table )
	
	return doc

def _makePatternObjectDict( pattern, type ):
	dict = {}
	for pc in pattern.childNodes:
		if pc.nodeName == "Input" or pc.nodeName == "Output":
			for obj in pc.childNodes:
				if obj.nodeName == type:
					dict[ obj.getAttribute( "name" ) ] = obj
	return dict

def _createPatternDocFile( node, targetDir, completeTree ):
	"creates an html documentation file for a given node"

	# create documentation string
	doc = _htmlFrame % { 'body': _makePatternDocumentation( node ), 'title': _prettyNodeName( node, False ) }

	# create file
	filename = os.path.join( targetDir, node.getAttribute( "name" ) + ".html" )
	print "Creating file " + filename
	f = file( filename, "w" )
	f.write( doc )
	f.close()
	
	# create pattern image
	if _haveGraphviz:
		_makePatternImage( node, os.path.join( targetDir, node.getAttribute( "name" ) + ".png" ) )

def _parseXml( filename ):
	"returns a list of 2-tuples ( <name>, <Pattern dom node> ) that contains all patterns in an xml file"
	
	from xml.dom.minidom import parse
	utqlDom = parse( filename )
	patternDoms = utqlDom.getElementsByTagName( 'Pattern' )
	r = map( lambda x: ( x.getAttribute( 'name' ), x ), patternDoms )
	return r
	
def _buildPatternsTree( dir ):
	"returns a recursive list of tuples containing either ( <dirname>, <sublist> ) or ( <patternname>, <dom node> )"
	
	currentLevel = []
	for p in glob.glob( os.path.join( dir, '*' ) ):
		if os.path.isdir( p ):
			subtree = _buildPatternsTree( os.path.join( dir, p ) )
			if len( subtree ) > 0:
				currentLevel.append( ( os.path.split( p )[ -1 ] , subtree ) )
		elif p.endswith( ".xml" ):
			print "parsing " + p
			try:
				subtree = _parseXml( os.path.join( dir, p ) )
				if len( subtree ) > 0:
					currentLevel += subtree
			except Exception, inst:
				print "ERROR PARSING " + p + ", line " + str(inst.lineno)
				
	return currentLevel

def _createDocTree( tree, targetDir, completeTree ):
	"writes the complete tree of pattern documentation files"

	for ( name, node ) in tree:
		if isinstance( node, list ):
			dir = os.path.join( targetDir, name )
			print "Creating directory " + dir
			if not os.path.exists( dir ):
				os.makedirs( dir )
			_createDocTree( node, dir, completeTree )
		else:
			_createPatternDocFile( node, targetDir, completeTree )

def _createTocFile( completeTree, targetDir ):
	"creates a table of contents"
	
	def makeTocString( tree, subdir ):
		toc = ""
		
		# first, write patterns in a directory, then subdirectories
		for ( name, node ) in tree:
			if not isinstance( node, list ):
				link = subdir + "/" + name + ".html"
				toc += _htmlTocLine % { 'link': link, 'title': _prettyNodeName( node, False ) }
				
		for ( name, node ) in tree:
			if isinstance( node, list ):
				level = len( subdir.split( "/" ) ) + 1
				toc += _htmlTocHeader % { 'level': level, 'name': name }
				toc += makeTocString( node, subdir + "/" + name )
				
		return toc
		
	# create documentation string
	doc = _htmlFrame % { 'body': makeTocString( completeTree, "." ), 'title': 'List of Patterns' }
	
	# create file
	filename = os.path.join( targetDir, "index.html" )
	print "Creating file " + filename
	f = file( filename, "w" )
	f.write( doc )
	f.close()
	
def makeDocumentation( sourceDir, targetDir ):
	# read tree of all patterns
	completeTree = _buildPatternsTree( sourceDir )
	
	# write pattern documentation
	_createDocTree( completeTree, targetDir, completeTree )
	
	# write table of contents
	_createTocFile( completeTree, targetDir )

basepath = os.path.abspath( os.path.dirname(sys.argv[0]) ) + os.sep + ".."
sourcepath = os.path.abspath( basepath + os.sep + "patterns" )
targetpath = os.path.abspath( basepath + os.sep + "patterns-doc")

makeDocumentation( sourcepath, targetpath )

