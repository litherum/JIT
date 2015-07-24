//
//  ASTParserHelpers.h
//  Regex
//
//  Created by Litherum on 7/24/15.
//  Copyright (c) 2015 Litherum. All rights reserved.
//

#ifndef __Regex__ASTParserHelpers__
#define __Regex__ASTParserHelpers__

typedef void* ASTNodeForParsing;

ASTNodeForParsing concatenation(ASTNodeForParsing, ASTNodeForParsing);
ASTNodeForParsing star(ASTNodeForParsing);
ASTNodeForParsing alternation(ASTNodeForParsing, ASTNodeForParsing);
ASTNodeForParsing literal(char);
ASTNodeForParsing epsilon();

#endif /* defined(__Regex__ASTParserHelpers__) */
