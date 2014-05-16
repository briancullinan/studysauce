<html>
<head></head>
<body style="word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space; color: rgb(0, 0, 0); font-size:14px; font-family: Calibri, sans-serif;">
<span class=""
      style="border-collapse: separate; font-family: Helvetica; font-style: normal; font-variant: normal; font-weight: normal; letter-spacing: normal; line-height: normal; orphans: 2; text-align: -webkit-auto; text-indent: 0px; text-transform: none; white-space: normal; widows: 2; word-spacing: 0px; -webkit-border-horizontal-spacing: 0px; -webkit-border-vertical-spacing: 0px; -webkit-text-decorations-in-effect: none; -webkit-text-size-adjust: auto; -webkit-text-stroke-width: 0px; font-size: medium; ">
    <div bgcolor="#ffffff"
         style="margin-top: 0px; margin-right: 0px; margin-bottom: 0px; margin-left: 0px; padding-top: 0px; padding-right: 0px; padding-bottom:0px; padding-left: 0px; ">
        <table width="100%" border="0" cellspacing="0" cellpadding="0" align="center">
            <tbody>
            <tr>
                <td style="padding-top: 20px; padding-right: 11px; padding-bottom: 40px; padding-left: 11px; background-color: rgb(255, 255, 255); ">
                    <table width="700" border="0" cellspacing="0" cellpadding="0" align="center"
                           bgcolor=""
                           style="margin-top: 0px; margin-right: auto; margin-bottom: 0px; margin-left: auto; background-color: rgb(255, 255, 255); ">
                        <tbody>
                        <tr>
                            <td width="700" valign="top">
                                <table width="648" border="0" cellspacing="0" cellpadding="0"
                                       align="center" bgcolor=""
                                       style="margin-top: 0px; margin-right: auto; margin-bottom: 0px; margin-left: auto;border-top-left-radius: 10px; border-top-right-radius: 10px; ">
                                    <tbody>
                                    <tr>
                                        <td><img
                                                src="<?php print url('<front>', array('absolute' => true, 'https' => false)); ?><?php print drupal_get_path('theme', 'successinc'); ?>/images/emails/top.gif"
                                                alt="" width="648" height="122" border="0"
                                                style="display: block; margin-top: 0px; margin-right: 0px; margin-bottom: 0px; margin-left: 0px; ">
                                        </td>
                                    </tr>
                                    </tbody>
                                </table>
                                <table width="630" border="0" cellspacing="0" cellpadding="0"
                                       align="center"
                                       style="margin-top: 0px; margin-right: auto; margin-bottom: 0px; margin-left: auto; border-bottom-left-radius: 10px; border-bottom-right-radius: 10px; background-image:url(<?php print url('<front>', array('absolute' => true, 'https' => false)); ?><?php print drupal_get_path('theme', 'successinc'); ?>/images/noise_gray.png); background-color:#EEEEEE; ">
                                    <tbody>
                                    <tr>
                                        <td>
                                            <table width="490" border="0" cellspacing="0"
                                                   cellpadding="0" align="center"
                                                   style="margin-top: 0px; margin-right: auto; margin-bottom: 0px; margin-left: auto; ">
                                                <tbody>
                                                <tr>
                                                    <td width="490" align="left"
                                                        style="padding-top: 0px; padding-right: 0px; padding-bottom: 22px; padding-left: 0px; ">
                                                        <div style="font-family: 'Lucida Grande', 'Lucida Sans', 'Lucida Sans Unicode', Arial, Helvetica, Verdana, sans-serif; color: rgb(51, 51, 51); font-size: 12px; line-height: 1.25em; ">
                                                            <span style="font-weight: bold; "><?php print t('Thanks for your order, !order_first_name!', array('!order_first_name' => $order_first_name)); ?>,</span><br><br>
                                                        </div>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2" bgcolor="#FF9900" style="color: white;">
                                                        <b><?php print t('Purchasing Information:'); ?></b>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td nowrap="nowrap">
                                                        <b><?php print t('E-mail Address:'); ?></b>
                                                    </td>
                                                    <td width="98%">
                                                        <?php print $order_email; ?>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td colspan="2">

                                                        <table width="100%" cellspacing="0" cellpadding="0" style="font-family: verdana, arial, helvetica; font-size: small;">
                                                            <tr>
                                                                <td valign="top" width="50%">
                                                                    <b><?php print t('Billing Address:'); ?></b><br />
                                                                    <?php print $order_billing_address; ?><br />
                                                                </td>
                                                                <?php if ($shippable): ?>
                                                                <td valign="top" width="50%">
                                                                    <b><?php print t('Shipping Address:'); ?></b><br />
                                                                    <?php print $order_shipping_address; ?><br />
                                                                    <br />
                                                                    <b><?php print t('Shipping Phone:'); ?></b><br />
                                                                    <?php print $order_shipping_phone; ?><br />
                                                                </td>
                                                                <?php endif; ?>
                                                            </tr>
                                                        </table>

                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td nowrap="nowrap">
                                                        <b><?php print t('Order Grand Total:'); ?></b>
                                                    </td>
                                                    <td width="98%">
                                                        <b><?php print $order_total; ?></b>
                                                    </td>
                                                </tr>
                                                <tr>
                                                    <td nowrap="nowrap">
                                                        <b><?php print t('Payment Method:'); ?></b>
                                                    </td>
                                                    <td width="98%">
                                                        <?php print $order_payment_method; ?>
                                                    </td>
                                                </tr>
                                                </tbody>
                                            </table>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td style="padding-top: 101px; "><img
                                                src="<?php print url('<front>', array('absolute' => true, 'https' => false)); ?><?php print drupal_get_path('theme', 'successinc'); ?>/images/emails/btm.gif"
                                                alt="" width="630" height="21" border="0"
                                                style="display: block; margin-top:0px; margin-right: 0px; margin-bottom: 0px; margin-left: 0px; ">
                                        </td>
                                    </tr>
                                    </tbody>
                                </table>
                                <table width="490" border="0" cellspacing="0" cellpadding="0"
                                       align="center" id="aapl-footer"
                                       style="margin-top: 0px; margin-right: auto;margin-bottom: 0px; margin-left: auto; ">
                                    <tbody>
                                    <tr>
                                        <td style="padding-top:10px; padding-right: 20px; padding-bottom: 10px; padding-left: 0px; ">
                                            <div style="font-family: Geneva, Verdana, Arial, Helvetica, sans-serif; font-size: 9px; line-height: 1.34em; color: rgb(153, 153, 153); ">
                                                Click here to use Study Sauce now.
                                            </div>
                                            <div style="font-family: Geneva, Verdana, Arial, Helvetica, sans-serif; font-size: 9px; line-height: 1.34em; color: rgb(153, 153, 153); ">
                                                <a target="_blank"
                                                    href="http://www.apple.com/legal/privacy/"
                                                    style="text-decoration: underline; color: rgb(153, 153, 153); font-family: Geneva, Verdana, Arial, Helvetica, sans-serif; font-size: 9px; line-height: 1.34em; ">Privacy
                                                Policy</a></div>
                                        </td>
                                    </tr>
                                    </tbody>
                                </table>
                            </td>
                        </tr>
                        </tbody>
                    </table>
                </td>
            </tr>
            </tbody>
        </table>
    </div>
</span>
</body>
</html>