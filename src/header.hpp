/*******************************************************************************
 * Wojciech Migda (https://www.linkedin.com/in/wojciechmigda)
 *
 * Literature:
 *   [1] Conditional Probabilistic Analysis for Prediction of the Activity Landscape and Relative Compound Activities
 *       Radleigh G. Santos, Marc A. Giulianotti, Richard A. Houghten, and José L. Medina-Franco
 *       Journal of Chemical Information and Modeling 2013 53 (10), 2613-2625
 *
 * From [1] I took the approach which uses conditional probability as a core
 * idea for relative activity prediction heuristics. Specifically,
 * Formulas 3 and 6. The former is represented by functions CP and CPsim, while
 * the later by APS and APSsim.
 *
 * There are other ways to tackle the problem, most of the are probably
 * summarized in this wikipedia page: https://en.wikipedia.org/wiki/Learning_to_rank
 *
 * Michiel Stock of the Ghent University in Belgium has been doing research
 * which part is focused on relative ordering of enzymes activity. Some of
 * his results are summarized in his Master Thesis:
 * http://lib.ugent.be/fulltxt/RUG01/001/894/310/RUG01-001894310_2012_0001_AC.pdf
 *
 * Unfortunately, I was unable to dive in and grasp entirety of his approach.
 ******************************************************************************/

