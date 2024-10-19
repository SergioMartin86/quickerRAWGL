nomalloc = environment({'MALLOC_PERTURB_': '0'})
testTimeout = 240

testSet = [ 
  'lvl01'
]

# Adding tests to the suite
foreach testFile : testSet
  test(testFile,
       quickerRAWTester,
       workdir : meson.current_source_dir(),
       timeout: testTimeout,
       args : [ testFile + '.test', testFile + '.sol' ],
  )
endforeach
