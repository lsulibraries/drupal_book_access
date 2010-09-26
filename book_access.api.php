<?php
// $Id$

/**
 * @file
 * Hooks provided by the Book access module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Allows other modules to change the author access permissions for a book.
 *
 * @param $grants
 *   The array of the grants, in the format @code $grants[$grant] @endcode.
 * @param $context
 *   An array containing two indexes:
 *   - the book ID (bid)
 *   - the node for the book page (node)
 */
 function hook_book_access_author_grants_alter(&$grants, $context) {
 }

/**
 * Allows other modules to change the users access permissions for a book.
 *
 * @param $uids
 *   The array containing the list of the user IDs.
 * @param $grants
 *   The array of the grants, in the format @code $grants[$grant][$uid] @endcode.
 * @param $context
 *   An array containing two indexes:
 *   - the book ID (bid)
 *   - the node for the book page (node)
 */
 function hook_book_access_users_grants_alter(&$uids, &$grants, $context) {
 }

/**
 * Allows other modules to change the roles access permissions for a book.
 *
 * @param $rids
 *   The array containing the list of the role IDs, as returned by
 *   user_roles().
 * @param $grants
 *   The array of the grants, in the format @code $grants[$grant][$rid] @endcode.
 * @param $context
 *   An array containing two indexes:
 *   - the book ID (bid)
 *   - the node for the book page (node)
 */
 function hook_book_access_users_grants_alter(&$rids, &$grants, $context) {
 }

/**
 * @} End of "addtogroup hooks".
 */
