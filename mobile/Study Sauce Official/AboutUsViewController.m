//
//  AboutUsViewController.m
//  Study Sauce Official
//
//  Created by Harry Alberti on 2/24/14.
//  Copyright (c) 2014 Harry Alberti. All rights reserved.
//

#import "AboutUsViewController.h"

@interface AboutUsViewController ()

@end

@implementation AboutUsViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    _youtube.backgroundColor = [UIColor clearColor];
    _youtube.opaque = NO;
    
    NSString* embedHTML = @"<iframe width=\"250\" height=\"141\" src=\"http://www.youtube.com/embed/vJG9PDaXNaQ\" frameborder=\"0\" allowfullscreen></iframe>";
    [_youtube loadHTMLString:embedHTML baseURL:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
@end
