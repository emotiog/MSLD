//
//  OpenCVWrapper.h
//  MSLD
//
//  Created by junsang on 2021/01/09.
//

#import <Foundation/Foundation.h>

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface OpenCVWrapper : NSObject


+(UIImage*) convertToGray:(UIImage*) image;
+(UIImage *) convertToCannyEdge:(UIImage *)image;
// 파라메터 선언 순서 : interface 노출명:(타입) 함수내 param명
// 예) isRandomColor 노출 - 함수 내에선 isRandom라는 명으로 사용
+(UIImage*) detectLine:(UIImage*) inputImage isRandomColor:(bool) isRandom hasDescriptor:(bool) hasDescriptor;

+(void) initDescManager:(NSString*) path;
+(void) appendDesc:(UIImage*) image savePath:(NSString*) path;
@end

NS_ASSUME_NONNULL_END
