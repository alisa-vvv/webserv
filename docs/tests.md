

==TEST A: CONFIG MATCHING==
testa1: no config match
testa2: no location match

TEST B: AUTOINDEX
testb1: autoindex on autoindex off


==TEST C: CLIENT MAX BODY SIZE==
testc1: client_max_body_size defaults to -1 -> if it is not initialized, is that ok? or will cnfig validation throw an error 

testc2: content is bigger than client max body size

test3: 


TEST D: Error page
testd1: Error page not found - go to default

testd2: Error page found - reroute to non-default