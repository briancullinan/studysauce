//
//  Google+ Login.m
//  Study Sauce Official
//
//  Created by Harry Alberti on 3/12/14.
//  Copyright (c) 2014 Harry Alberti. All rights reserved.
//

#import "Google+ Login.h"

@interface Google__Login ()

@end

@implementation Google__Login

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
    
    [_emailField setDelegate:(id)self];
    [_PasswordField setDelegate:(id)self];
    [self.view addSubview:_emailField];
}

-(BOOL)textFieldShouldReturn:(UITextField*)textField;
{
    _emailField.tag = 0;
    _PasswordField.tag = 1;
    NSInteger nextTag = textField.tag + 1;
    // Try to find next responder
    UIResponder* nextResponder = [textField.superview viewWithTag:nextTag];
    if (nextResponder) {
        // Found next responder, so set it.
        [nextResponder becomeFirstResponder];
    } else {
        // Not found, so remove keyboard.
        [textField resignFirstResponder];
    }
    return NO; // We do not want UITextField to insert line-breaks.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
